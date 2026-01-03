// Copyright Yureka. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FileTreeIconManager.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"

DECLARE_DELEGATE_OneParam(FOnFileSelected, const FString & /*FilePath*/);
DECLARE_DELEGATE(FOnSimpleAction);

/**
 * Represents a single item in the file tree (file or folder)
 */
class FFileTreeItem : public TSharedFromThis<FFileTreeItem> {
public:
  FFileTreeItem(const FString &InPath, const FString &InName,
                bool bInIsDirectory)
      : Path(InPath), DisplayName(InName), bIsDirectory(bInIsDirectory),
        bIsExpanded(false) {}

  /** Full path to this item */
  FString Path;

  /** Display name shown in the tree */
  FString DisplayName;

  /** Whether this is a directory */
  bool bIsDirectory;

  /** Whether the directory is expanded */
  bool bIsExpanded;

  /** Child items (for directories) */
  TArray<TSharedPtr<FFileTreeItem>> Children;

  /** Parent item */
  TWeakPtr<FFileTreeItem> Parent;

  /** Get file extension */
  FString GetExtension() const {
    return bIsDirectory ? TEXT("") : FPaths::GetExtension(Path).ToLower();
  }

  /** Check if children have been loaded */
  bool HasLoadedChildren() const { return bChildrenLoaded; }

  /** Mark children as loaded */
  void SetChildrenLoaded(bool bLoaded) { bChildrenLoaded = bLoaded; }

private:
  bool bChildrenLoaded = false;
};

/**
 * File tree view widget for browsing project files
 * Provides a VSCode-like file explorer experience
 */
class SFileTreeView : public SCompoundWidget {
public:
  SLATE_BEGIN_ARGS(SFileTreeView) : _RootPath(TEXT("")) {}
  /** Root path to display in the tree */
  SLATE_ARGUMENT(FString, RootPath)

  /** Called when a file is double-clicked */
  SLATE_EVENT(FOnFileSelected, OnFileDoubleClicked)

  /** Called when Open Folder button is clicked */
  SLATE_EVENT(FOnSimpleAction, OnOpenFolderClicked)

  /** Called when Toggle Explorer button is clicked */
  SLATE_EVENT(FOnSimpleAction, OnToggleExplorerClicked)
  SLATE_END_ARGS()

  void Construct(const FArguments &InArgs);

  /** Set the root path and refresh the tree */
  void SetRootPath(const FString &NewRootPath);

  /** Refresh the entire tree from disk */
  void RefreshTree();

  /** Expand all folders */
  void ExpandAll();

  /** Collapse all folders */
  void CollapseAll();

  /** Get currently selected file path (empty if none or folder selected) */
  FString GetSelectedFilePath() const;

private:
  /** Build tree items from a directory */
  void BuildTreeFromDirectory(const FString &DirectoryPath);

  /** Populate children for a directory item */
  void PopulateChildren(TSharedPtr<FFileTreeItem> DirectoryItem);

  /** Generate a row for the tree view */
  TSharedRef<ITableRow>
  OnGenerateRow(TSharedPtr<FFileTreeItem> Item,
                const TSharedRef<STableViewBase> &OwnerTable);

  /** Get children for a tree item */
  void OnGetChildren(TSharedPtr<FFileTreeItem> Item,
                     TArray<TSharedPtr<FFileTreeItem>> &OutChildren);

  /** Handle item double click */
  void OnItemDoubleClicked(TSharedPtr<FFileTreeItem> Item);

  /** Handle item expansion change */
  void OnExpansionChanged(TSharedPtr<FFileTreeItem> Item, bool bExpanded);

  /** Handle selection change */
  void OnSelectionChanged(TSharedPtr<FFileTreeItem> Item,
                          ESelectInfo::Type SelectInfo);

  /** Get the icon for a file type */
  const FSlateBrush *GetIconForItem(TSharedPtr<FFileTreeItem> Item) const;

  /** Get the icon color for a file type */
  FSlateColor GetIconColorForItem(TSharedPtr<FFileTreeItem> Item) const;

  /** Get text color for an item */
  FSlateColor GetTextColorForItem(TSharedPtr<FFileTreeItem> Item) const;

  /** Handle refresh button click */
  FReply OnRefreshClicked();

  /** Handle collapse all button click */
  FReply OnCollapseAllClicked();

private:
  /** Current root path */
  FString RootPath;

  /** Root items in the tree */
  TArray<TSharedPtr<FFileTreeItem>> RootItems;

  /** The tree view widget */
  TSharedPtr<STreeView<TSharedPtr<FFileTreeItem>>> TreeView;

  /** Currently selected item */
  TSharedPtr<FFileTreeItem> SelectedItem;

  /** Callback for file double-click */
  FOnFileSelected OnFileDoubleClicked;

  /** Root folder display name */
  TSharedPtr<STextBlock> RootFolderNameText;

  /** Callback for Open Folder button */
  FOnSimpleAction OnOpenFolderClicked;

  /** Callback for Toggle Explorer button */
  FOnSimpleAction OnToggleExplorerClicked;
};
