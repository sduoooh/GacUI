#include "GuiFakeDialogServiceBase.h"
#include "../../Application/Controls/GuiApplication.h"

namespace vl
{
	namespace presentation
	{
		using namespace collections;
		using namespace regex;
		using namespace controls;

/***********************************************************************
View Model (IFileDialogFilter)
***********************************************************************/

		class FileDialogFilter : public Object, public virtual IFileDialogFilter
		{
		public:
			WString						name;
			WString						filter;

			WString GetName() override
			{
				return name;
			}

			WString GetFilter() override
			{
				return filter;
			}
		};

/***********************************************************************
View Model (IFileDialogFolder)
***********************************************************************/

		class FileDialogFolder : public Object, public virtual IFileDialogFolder
		{
		protected:
			vint						taskId = 0;
			bool						taskFired = false;

		public:
			FileDialogFolder*			parent = nullptr;
			FileDialogFolderType		type = FileDialogFolderType::Placeholder;
			filesystem::Folder			folder;
			WString						name;
			WString						textLoadingFolders;
			Folders						children;

			void AddChild(Ptr<FileDialogFolder> child)
			{
				child->textLoadingFolders = textLoadingFolders;
				child->parent = this;
				children.Add(child);
			}

			void AddPlaceholderChild()
			{
				auto child = Ptr(new FileDialogFolder);
				child->name = textLoadingFolders;
				AddChild(child);
			}

			FileDialogFolder() = default;

			FileDialogFolder(const filesystem::FilePath& filePath)
				: folder(filePath)
			{
			}

			Ptr<IFileDialogFolder> GetParent() override
			{
				return Ptr(static_cast<IFileDialogFolder*>(parent));
			}

			FileDialogFolderType GetType() override
			{
				if (type == FileDialogFolderType::Placeholder && !taskFired)
				{
					taskFired = true;
					auto taskFolder = Ptr(parent);
					vint taskFolderId = ++taskFolder->taskId;
					GetApplication()->InvokeAsync([taskFolder, taskFolderId]()
					{
						auto subFolders = Ptr(new List<filesystem::Folder>);
						if (!taskFolder->folder.GetFolders(*subFolders.Obj()))
						{
							subFolders->Clear();
						}

						GetApplication()->InvokeInMainThread(nullptr, [taskFolder, taskFolderId, subFolders]()
						{
							if (taskFolder->taskId == taskFolderId)
							{
								taskFolder->children.Clear();
								for (auto subFolder : *subFolders.Obj())
								{
									auto child = Ptr(new FileDialogFolder);
									child->type = FileDialogFolderType::Folder;
									child->folder = subFolder;
									child->name = subFolder.GetFilePath().GetName();
									child->AddPlaceholderChild();
									taskFolder->AddChild(child);
								}
							}
						});
					});
				}
				return type;
			}

			WString GetFullPath() override
			{
				return folder.GetFilePath().GetFullPath();
			}

			WString GetName() override
			{
				return name;
			}

			Folders& GetFolders() override
			{
				return children;
			}
		};

/***********************************************************************
View Model (IFileDialogFile)
***********************************************************************/

		class FileDialogFile : public Object, public virtual IFileDialogFile
		{
		public:
			FileDialogFileType			type = FileDialogFileType::Placeholder;
			Ptr<FileDialogFolder>		folder;
			WString						name;

			FileDialogFileType GetType() override
			{
				return type;
			}

			Ptr<IFileDialogFolder> GetAssociatedFolder() override
			{
				return folder;
			}

			WString GetName() override
			{
				return name;
			}
		};

/***********************************************************************
View Model (IFileDialogViewModel)
***********************************************************************/

		class FileDialogViewModel : public Object, public virtual IFileDialogViewModel
		{
			using ConfirmedSelection = collections::List<WString>;
		protected:
			WString						textLoadingFolders;
			WString						textLoadingFiles;
			WString						dialogErrorEmptySelection;
			WString						dialogErrorFileNotExist;
			WString						dialogErrorFileExpected;
			WString						dialogErrorFolderNotExist;
			WString						dialogErrorMultipleSelectionNotEnabled;
			WString						dialogAskCreateFile;
			WString						dialogAskOverrideFile;

			bool						isLoadingFiles = false;
			filesystem::FilePath		loadingPath;
			vint						loadingTaskId = 0;
			Files						files;
			Regex						regexDisplayString{ L";" };

		public:
			bool						selectToSave = false;
			bool						confirmed = false;
			ConfirmedSelection			confirmedSelection;

			WString						title;
			bool						enabledMultipleSelection = false;
			bool						fileMustExist = false;
			bool						folderMustExist = false;
			bool						promptCreateFile = false;
			bool						promptOverriteFile = false;
			WString						defaultExtension;

			Filters						filters;
			Ptr<FileDialogFilter>		selectedFilter;

			Ptr<FileDialogFolder>		rootFolder;
			Ptr<FileDialogFolder>		selectedFolder;

			WString GetTitle() override
			{
				return title;
			}

			bool GetEnabledMultipleSelection() override
			{
				return enabledMultipleSelection;
			}

			WString GetDefaultExtension() override
			{
				return defaultExtension;
			}

			const Filters& GetFilters() override
			{
				return filters;
			}

			Ptr<IFileDialogFilter> GetSelectedFilter() override
			{
				return selectedFilter;
			}

			void SetSelectedFilter(Ptr<IFileDialogFilter> value) override
			{
				if (auto filter = value.Cast<FileDialogFilter>())
				{
					if (selectedFilter != filter && filters.Contains(value.Obj()))
					{
						selectedFilter = filter;
						RefreshFiles();
					}
				}
			}

			Ptr<IFileDialogFolder> GetRootFolder() override
			{
				return rootFolder;
			}

			Ptr<IFileDialogFolder> GetSelectedFolder() override
			{
				return selectedFolder;
			}

			void SetSelectedFolder(Ptr<IFileDialogFolder> value) override
			{
				if (auto folder = value.Cast<FileDialogFolder>())
				{
					if (selectedFolder != folder)
					{
						selectedFolder = folder;
						SelectedFolderChanged();

						if (loadingPath != selectedFolder->folder.GetFilePath())
						{
							RefreshFiles();
						}
					}
				}
			}

			bool GetIsLoadingFiles() override
			{
				return isLoadingFiles;
			}

			Files& GetFiles() override
			{
				return files;
			}

			void RefreshFiles() override
			{
				if (!selectedFolder) return;

				vint taskId = ++loadingTaskId;
				auto taskFolder = selectedFolder;
				auto taskPath = (loadingPath = selectedFolder->folder.GetFilePath());
				if (!isLoadingFiles)
				{
					isLoadingFiles = true;
					IsLoadingFilesChanged();

					files.Clear();
					{
						auto item = Ptr(new FileDialogFile);
						item->name = textLoadingFiles;
						files.Add(item);
					}
				}

				auto vm = Ptr(this);
				GetApplication()->InvokeAsync([taskId, taskFolder, taskPath, vm]()
				{
					auto folders = Ptr(new List<filesystem::Folder>);
					auto files = Ptr(new List<filesystem::File>);
					filesystem::Folder loadingFolder = taskPath;
					if (!loadingFolder.GetFolders(*folders.Obj()) || !loadingFolder.GetFiles(*files.Obj()))
					{
						folders->Clear();
						files->Clear();
					}

					GetApplication()->InvokeInMainThread(nullptr, [taskId, taskFolder, folders, files, vm]()
					{
						if (vm->loadingTaskId == taskId)
						{
							vm->files.Clear();

							for (auto folder : *folders.Obj())
							{
								auto item = Ptr(new FileDialogFile);
								item->type = FileDialogFileType::Folder;
								item->folder = taskFolder;
								item->name = folder.GetFilePath().GetName();
								vm->files.Add(item);
							}

							for (auto file : *files.Obj())
							{
								auto item = Ptr(new FileDialogFile);
								item->type = FileDialogFileType::File;
								item->folder = taskFolder;
								item->name = file.GetFilePath().GetName();
								vm->files.Add(item);
							}

							vm->isLoadingFiles = false;
							vm->IsLoadingFilesChanged();
						}
					});
				});
			}

			WString GetDisplayString(collections::LazyList<Ptr<IFileDialogFile>> files) override
			{
				return From(files)
					.FindType<FileDialogFile>()
					.Where([](auto file) { return file->type != FileDialogFileType::Placeholder; })
					.Select([](auto file) { return WString::Unmanaged(L"\"") + file->name + WString::Unmanaged(L"\""); })
					.Aggregate(WString::Empty, [](auto&& a, auto&& b)
					{
						return a == WString::Empty ? b : a + WString::Unmanaged(L";") + b;
					});
			}

			Selection ParseDisplayString(const WString& displayString) override
			{
				auto items = Ptr(new RegexMatch::List);
				regexDisplayString.Split(displayString, false, *items.Obj());
				return From(LazyList<Ptr<RegexMatch>>(items))
					.Select([](auto item) { return item->Result().Value(); })
					.Select([](auto item)
					{
						if (item.Length() >= 2 && item[0] == L'\"' && item[item.Length() - 1] == L'\"')
						{
							return item.Sub(1, item.Length() - 2);
						}
						else
						{
							return item;
						}
					});
			}

			bool TryConfirm(controls::GuiWindow* owner, Selection selectedPaths) override
			{
				auto wd = selectedFolder->folder.GetFilePath();
				List<filesystem::FilePath> paths;
				CopyFrom(
					paths,
					selectedPaths.Select([this, wd](auto path) { return wd / path; })
					);

				if (paths.Count() == 0)
				{
					GetCurrentController()->DialogService()->ShowMessageBox(
						owner->GetNativeWindow(),
						dialogErrorEmptySelection,
						owner->GetText(),
						INativeDialogService::DisplayOK,
						INativeDialogService::DefaultFirst,
						INativeDialogService::IconError
						);
					return false;
				}
				else if (paths.Count() == 1)
				{
					auto path = paths[0];
					{
						auto folder = filesystem::Folder(path);
						if (folder.Exists())
						{
							// TODO: jump to that folder
							CHECK_FAIL(L"Not Implemented!");
						}
					}
				}
				else
				{
					if (!enabledMultipleSelection)
					{
						GetCurrentController()->DialogService()->ShowMessageBox(
							owner->GetNativeWindow(),
							dialogErrorMultipleSelectionNotEnabled,
							owner->GetText(),
							INativeDialogService::DisplayOK,
							INativeDialogService::DefaultFirst,
							INativeDialogService::IconError
							);
						return false;
					}
				}

				List<vint> files, folders, unexistings;
				for (auto [path, index] : indexed(paths))
				{
					if (filesystem::File(path).Exists())
					{
						files.Add(index);
					}
					else if (filesystem::Folder(path).Exists())
					{
						folders.Add(index);
					}
					else
					{
						unexistings.Add(index);
					}
				}

				if (folders.Count() > 0)
				{
					auto message = stream::GenerateToStream([&](stream::TextWriter& writer)
					{
						writer.WriteString(dialogErrorFileExpected);
						for (vint index : folders)
						{
							writer.WriteLine(WString::Empty);
							writer.WriteString(L"  ");
							writer.WriteString(wd.GetRelativePathFor(paths[index]));
						}
					});
					GetCurrentController()->DialogService()->ShowMessageBox(
						owner->GetNativeWindow(),
						message,
						owner->GetText(),
						INativeDialogService::DisplayOK,
						INativeDialogService::DefaultFirst,
						INativeDialogService::IconError
						);
					return false;
				}

				if (unexistings.Count() > 0)
				{
					if (fileMustExist)
					{
						auto message = stream::GenerateToStream([&](stream::TextWriter& writer)
						{
							writer.WriteString(dialogErrorFileNotExist);
							for (vint index : unexistings)
							{
								writer.WriteLine(WString::Empty);
								writer.WriteString(L"  ");
								writer.WriteString(wd.GetRelativePathFor(paths[index]));
							}
						});
						GetCurrentController()->DialogService()->ShowMessageBox(
							owner->GetNativeWindow(),
							message,
							owner->GetText(),
							INativeDialogService::DisplayOK,
							INativeDialogService::DefaultFirst,
							INativeDialogService::IconError
							);
						return false;
					}

					if (folderMustExist)
					{
						SortedList<filesystem::FilePath> folderOfUnexistings;
						for (vint index : unexistings)
						{
							auto path = paths[index].GetFolder();
							if(!folderOfUnexistings.Contains(path))
							{
								folderOfUnexistings.Add(path);
							}
						}

						for (vint i = folderOfUnexistings.Count() - 1; i >= 0; i--)
						{
							if (filesystem::Folder(folderOfUnexistings[i]).Exists())
							{
								folderOfUnexistings.RemoveAt(i);
							}
						}

						if (folderOfUnexistings.Count() > 0)
						{
							auto message = stream::GenerateToStream([&](stream::TextWriter& writer)
							{
								writer.WriteString(dialogErrorFolderNotExist);
								for (auto path : folderOfUnexistings)
								{
									writer.WriteLine(WString::Empty);
									writer.WriteString(L"  ");
									writer.WriteString(wd.GetRelativePathFor(path));
								}
							});
							GetCurrentController()->DialogService()->ShowMessageBox(
								owner->GetNativeWindow(),
								message,
								owner->GetText(),
								INativeDialogService::DisplayOK,
								INativeDialogService::DefaultFirst,
								INativeDialogService::IconError
								);
							return false;
						}
					}

					WString question;
					if (selectToSave && promptOverriteFile)
					{
						question = dialogAskOverrideFile;
					}
					if (!selectToSave && promptCreateFile)
					{
						question = dialogAskCreateFile;
					}

					if ((selectToSave && promptOverriteFile) || (!selectToSave && promptCreateFile))
					{
						auto message = stream::GenerateToStream([&](stream::TextWriter& writer)
						{
							writer.WriteString(question);
							for (vint index : unexistings)
							{
								writer.WriteLine(WString::Empty);
								writer.WriteString(L"  ");
								writer.WriteString(wd.GetRelativePathFor(paths[index]));
							}
						});

						auto result = GetCurrentController()->DialogService()->ShowMessageBox(
							owner->GetNativeWindow(),
							message,
							owner->GetText(),
							INativeDialogService::DisplayOKCancel,
							INativeDialogService::DefaultThird,
							INativeDialogService::IconQuestion
							);

						if (result == INativeDialogService::SelectCancel)
						{
							return false;
						}
					}
				}

				CopyFrom(
					confirmedSelection,
					From(paths).Select([](auto path) { return path.GetFullPath(); })
					);
				confirmed = true;
				return true;
			}

			void InitLocalizedText(
				const WString& _textLoadingFolders,
				const WString& _textLoadingFiles,
				const WString& _dialogErrorEmptySelection,
				const WString& _dialogErrorFileNotExist,
				const WString& _dialogErrorFileExpected,
				const WString& _dialogErrorFolderNotExist,
				const WString& _dialogErrorMultipleSelectionNotEnabled,
				const WString& _dialogAskCreateFile,
				const WString& _dialogAskOverrideFile
			) override
			{
				textLoadingFolders = _textLoadingFolders;
				textLoadingFiles = _textLoadingFiles;
				dialogErrorEmptySelection = _dialogErrorEmptySelection;
				dialogErrorFileNotExist = _dialogErrorFileNotExist;
				dialogErrorFileExpected = _dialogErrorFileExpected;
				dialogErrorFolderNotExist = _dialogErrorFolderNotExist;
				dialogErrorMultipleSelectionNotEnabled = _dialogErrorMultipleSelectionNotEnabled;
				dialogAskCreateFile = _dialogAskCreateFile;
				dialogAskOverrideFile = _dialogAskOverrideFile;

				rootFolder->textLoadingFolders = textLoadingFolders;
				rootFolder->AddPlaceholderChild();
			}
		};

/***********************************************************************
FakeDialogServiceBase
***********************************************************************/

		bool FakeDialogServiceBase::ShowFileDialog(
			INativeWindow* window,
			collections::List<WString>& selectionFileNames,
			vint& selectionFilterIndex,
			FileDialogTypes dialogType,
			const WString& title,
			const WString& initialFileName,
			const WString& initialDirectory,
			const WString& defaultExtension,
			const WString& filter,
			FileDialogOptions options
		)
		{
			auto vm = Ptr(new FileDialogViewModel);
			vm->title = title;
			vm->enabledMultipleSelection = (options | INativeDialogService::FileDialogAllowMultipleSelection) != 0;
			vm->fileMustExist = (options | INativeDialogService::FileDialogFileMustExist) != 0;
			vm->folderMustExist = (options | INativeDialogService::FileDialogDirectoryMustExist) != 0;
			vm->promptCreateFile = (options | INativeDialogService::FileDialogPromptCreateFile) != 0;
			vm->promptOverriteFile = (options | INativeDialogService::FileDialogPromptOverwriteFile) != 0;
			vm->defaultExtension = defaultExtension;

			vint filterStart = 0;
			while (true)
			{
				vint first = -1;
				vint second = -1;
				vint count = filter.Length();

				for (vint i = filterStart; i < count; i++)
				{
					if (filter[i] == L'|')
					{
						first = i;
						break;
					}
				}
				if (first == -1) break;

				for (vint i = first + 1; i < count; i++)
				{
					if (filter[i] == L'|')
					{
						second = i;
						break;
					}
				}

				auto filterItem = Ptr(new FileDialogFilter);
				filterItem->name = filter.Sub(filterStart, first - filterStart);
				filterItem->filter = filter.Sub(first + 1, (second == -1 ? count : second) - first - 1);
				vm->filters.Add(filterItem);

				if (second == -1) break;
				filterStart = second + 1;
			}

			if (vm->filters.Count() > 0)
			{
				if (selectionFilterIndex < 0 || vm->filters.Count() <= selectionFilterIndex)
				{
					selectionFilterIndex = 0;
				}
				vm->selectedFilter = vm->filters[selectionFilterIndex].Cast<FileDialogFilter>();
			}

			vm->rootFolder = Ptr(new FileDialogFolder);
			vm->rootFolder->type = FileDialogFolderType::Root;

			// TODO: initialDirectory -> selectedFolder
			vm->SetSelectedFolder(vm->rootFolder);

			switch (dialogType)
			{
			case INativeDialogService::FileDialogOpen:
			case INativeDialogService::FileDialogOpenPreview:
				{
					vm->selectToSave = false;
					auto owner = GetApplication()->GetWindowFromNative(window);
					auto dialog = CreateOpenFileDialog(vm);
					ShowModalDialogAndDelete(vm, owner, dialog);
				}
				break;
			case INativeDialogService::FileDialogSave:
			case INativeDialogService::FileDialogSavePreview:
				{
					vm->selectToSave = true;
					auto owner = GetApplication()->GetWindowFromNative(window);
					auto dialog = CreateSaveFileDialog(vm);
					ShowModalDialogAndDelete(vm, owner, dialog);
				}
				break;
			}

			if (vm->filters.Count() > 0)
			{
				selectionFilterIndex = vm->filters.IndexOf(vm->selectedFilter.Obj());
			}

			if (vm->confirmed)
			{
				CopyFrom(selectionFileNames, vm->confirmedSelection);
			}
			return vm->confirmed;
		}
	}
}