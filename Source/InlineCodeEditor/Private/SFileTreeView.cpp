// Copyright Yureka. All Rights Reserved.

#include "SFileTreeView.h"
#include "FileTreeIconManager.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Styling/AppStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableRow.h"

#define LOCTEXT_NAMESPACE "SFileTreeView"

// VSCode-like color scheme - use FromSRGBColor for correct color matching
namespace FileTreeColors {
// Background colors - MUST use FromSRGBColor to match code editor
const FLinearColor TreeBackground =
    FLinearColor::FromSRGBColor(FColor::FromHex("1E1E1EFF")); // Dark background
const FLinearColor HeaderBackground =
    FLinearColor::FromSRGBColor(FColor::FromHex("1E1E1EFF")); // Same as tree
const FLinearColor RowHoverBackground = FLinearColor::FromSRGBColor(
    FColor::FromHex("2A2D2EFF")); // Slightly lighter
const FLinearColor RowSelectedBackground =
    FLinearColor::FromSRGBColor(FColor::FromHex("094771FF")); // Blue selection

// Text colors
const FLinearColor TextNormal = FLinearColor(0.8f, 0.8f, 0.8f); // #CCCCCC
const FLinearColor TextFolder =
    FLinearColor(0.878f, 0.788f, 0.557f); // #E0C98E - Gold
const FLinearColor TextSelected = FLinearColor(1.0f, 1.0f, 1.0f); // #FFFFFF

// File type colors (matching VSCode icons)
const FLinearColor IconFolder =
    FLinearColor(0.878f, 0.788f, 0.557f); // #E0C98E - Gold
const FLinearColor IconFolderOpen =
    FLinearColor(0.929f, 0.867f, 0.51f); // #EDDD82 - Light Gold
const FLinearColor IconCpp = FLinearColor(0.0f, 0.478f, 0.8f); // #007ACC - Blue
const FLinearColor IconHeader =
    FLinearColor(0.608f, 0.318f, 0.878f); // #9B51E0 - Purple
const FLinearColor IconCSharp =
    FLinearColor(0.408f, 0.608f, 0.078f); // #689B14 - Green
const FLinearColor IconJson =
    FLinearColor(0.988f, 0.82f, 0.282f); // #FCD148 - Yellow
const FLinearColor IconText = FLinearColor(0.6f, 0.6f, 0.6f); // #999999 - Gray
const FLinearColor IconConfig =
    FLinearColor(0.4f, 0.6f, 0.8f); // #6699CC - Light Blue
const FLinearColor IconImage =
    FLinearColor(0.886f, 0.431f, 0.278f); // #E26E47 - Orange
const FLinearColor IconDefault =
    FLinearColor(0.7f, 0.7f, 0.7f); // #B3B3B3 - Light Gray
} // namespace FileTreeColors

void SFileTreeView::Construct(const FArguments &InArgs) {
  RootPath = InArgs._RootPath;
  OnFileDoubleClicked = InArgs._OnFileDoubleClicked;
  OnOpenFolderClicked = InArgs._OnOpenFolderClicked;
  OnToggleExplorerClicked = InArgs._OnToggleExplorerClicked;

  ChildSlot
      [SNew(SBorder)
           .BorderImage(FAppStyle::GetBrush("NoBorder"))
           .ColorAndOpacity(FLinearColor::White)
           .Padding(0)
               [SNew(SVerticalBox)

                // Header with title and buttons
                +
                SVerticalBox::Slot().AutoHeight()
                    [SNew(SBorder)
                         .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                         .BorderBackgroundColor(
                             FileTreeColors::HeaderBackground)
                         .Padding(FMargin(8.0f, 6.0f))
                             [SNew(SHorizontalBox)

                              // Explorer title
                              + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(
                                    VAlign_Center)
                                    [SNew(STextBlock)
                                         .Text(LOCTEXT("ExplorerTitle", "EXPLOR"
                                                                        "ER"))
                                         .Font(FCoreStyle::GetDefaultFontStyle(
                                             "Bold", 10))
                                         .ColorAndOpacity(FSlateColor(
                                             FileTreeColors::TextNormal))]

                              // Refresh button
                              +
                              SHorizontalBox::Slot().AutoWidth().Padding(2.0f,
                                                                         0.0f)
                                  [SNew(SButton)
                                       .ButtonStyle(FAppStyle::Get(), "FlatButt"
                                                                      "on")
                                       .ContentPadding(FMargin(2.0f))
                                       .OnClicked(this, &SFileTreeView::
                                                            OnRefreshClicked)
                                       .ToolTipText(
                                           LOCTEXT("RefreshTooltip", "Refresh"))
                                           [SNew(SImage)
                                                .Image(FAppStyle::GetBrush(
                                                    "Icons.Refresh"))
                                                .ColorAndOpacity(FSlateColor(
                                                    FileTreeColors::TextNormal))
                                                .DesiredSizeOverride(
                                                    FVector2D(14.0f, 14.0f))]]

                              // Collapse all button
                              +
                              SHorizontalBox::Slot().AutoWidth().Padding(2.0f,
                                                                         0.0f)
                                  [SNew(SButton)
                                       .ButtonStyle(FAppStyle::Get(), "FlatButt"
                                                                      "on")
                                       .ContentPadding(FMargin(2.0f))
                                       .OnClicked(
                                           this,
                                           &SFileTreeView::OnCollapseAllClicked)
                                       .ToolTipText(LOCTEXT("CollapseTooltip",
                                                            "Collapse All"))
                                           [SNew(SImage)
                                                .Image(FAppStyle::GetBrush(
                                                    "TreeArrow_Collapsed"))
                                                .ColorAndOpacity(FSlateColor(
                                                    FileTreeColors::TextNormal))
                                                .DesiredSizeOverride(
                                                    FVector2D(14.0f, 14.0f))]]]]

                // Root folder name
                +
                SVerticalBox::Slot().AutoHeight()
                    [SNew(SBorder)
                         .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                         .BorderBackgroundColor(FileTreeColors::TreeBackground)
                         .Padding(FMargin(8.0f, 4.0f))
                             [SNew(SHorizontalBox)

                              + SHorizontalBox::Slot()
                                    .AutoWidth()
                                    .VAlign(VAlign_Center)
                                    .Padding(0.0f, 0.0f, 4.0f, 0.0f)
                                        [SNew(SImage)
                                             .Image(FAppStyle::GetBrush(
                                                 "TreeArrow_Expanded"))
                                             .ColorAndOpacity(FSlateColor(
                                                 FileTreeColors::TextNormal))]

                              +
                              SHorizontalBox::Slot().FillWidth(1.0f).VAlign(
                                  VAlign_Center)
                                  [SAssignNew(RootFolderNameText, STextBlock)
                                       .Text(FText::FromString(
                                           FPaths::GetCleanFilename(RootPath)))
                                       .Font(FCoreStyle::GetDefaultFontStyle(
                                           "Bold", 11))
                                       .ColorAndOpacity(FSlateColor(
                                           FileTreeColors::TextFolder))]]]

                // Tree view
                +
                SVerticalBox::Slot().FillHeight(1.0f)
                    [SNew(SBorder)
                         .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                         .BorderBackgroundColor(FileTreeColors::TreeBackground)
                         .Padding(FMargin(4.0f, 0.0f))
                             [SAssignNew(TreeView,
                                         STreeView<TSharedPtr<FFileTreeItem>>)
                                  .TreeItemsSource(&RootItems)
                                  .OnGenerateRow(this,
                                                 &SFileTreeView::OnGenerateRow)
                                  .OnGetChildren(this,
                                                 &SFileTreeView::OnGetChildren)
                                  .OnMouseButtonDoubleClick(
                                      this, &SFileTreeView::OnItemDoubleClicked)
                                  .OnExpansionChanged(
                                      this, &SFileTreeView::OnExpansionChanged)
                                  .OnSelectionChanged(
                                      this, &SFileTreeView::OnSelectionChanged)
                                  .SelectionMode(ESelectionMode::Single)]]]];

  // Build initial tree if path is set
  FFileTreeIconManager::Get().Initialize();
  if (!RootPath.IsEmpty()) {
    BuildTreeFromDirectory(RootPath);
  }
}

void SFileTreeView::SetRootPath(const FString &NewRootPath) {
  RootPath = NewRootPath;

  if (RootFolderNameText.IsValid()) {
    RootFolderNameText->SetText(
        FText::FromString(FPaths::GetCleanFilename(RootPath)));
  }

  RefreshTree();
}

void SFileTreeView::RefreshTree() { BuildTreeFromDirectory(RootPath); }

void SFileTreeView::BuildTreeFromDirectory(const FString &DirectoryPath) {
  RootItems.Empty();

  if (DirectoryPath.IsEmpty() || !FPaths::DirectoryExists(DirectoryPath)) {
    if (TreeView.IsValid()) {
      TreeView->RequestTreeRefresh();
    }
    return;
  }

  // Get all items in the root directory
  TArray<FString> Files;
  TArray<FString> Directories;

  IFileManager &FileManager = IFileManager::Get();

  // Find directories
  FileManager.FindFiles(Directories, *(DirectoryPath / TEXT("*")), false, true);

  // Find files
  FileManager.FindFiles(Files, *(DirectoryPath / TEXT("*")), true, false);

  // Sort alphabetically (directories first)
  Directories.Sort();
  Files.Sort();

  // Create directory items
  for (const FString &DirName : Directories) {
    // Skip hidden directories
    if (DirName.StartsWith(TEXT("."))) {
      continue;
    }

    FString FullPath = DirectoryPath / DirName;
    TSharedPtr<FFileTreeItem> Item =
        MakeShared<FFileTreeItem>(FullPath, DirName, true);
    RootItems.Add(Item);
  }

  // Create file items
  for (const FString &FileName : Files) {
    // Skip hidden files
    if (FileName.StartsWith(TEXT("."))) {
      continue;
    }

    FString FullPath = DirectoryPath / FileName;
    TSharedPtr<FFileTreeItem> Item =
        MakeShared<FFileTreeItem>(FullPath, FileName, false);
    RootItems.Add(Item);
  }

  if (TreeView.IsValid()) {
    TreeView->RequestTreeRefresh();
  }
}

void SFileTreeView::PopulateChildren(TSharedPtr<FFileTreeItem> DirectoryItem) {
  if (!DirectoryItem.IsValid() || !DirectoryItem->bIsDirectory ||
      DirectoryItem->HasLoadedChildren()) {
    return;
  }

  DirectoryItem->Children.Empty();

  TArray<FString> Files;
  TArray<FString> Directories;

  IFileManager &FileManager = IFileManager::Get();

  // Find directories
  FileManager.FindFiles(Directories, *(DirectoryItem->Path / TEXT("*")), false,
                        true);

  // Find files
  FileManager.FindFiles(Files, *(DirectoryItem->Path / TEXT("*")), true, false);

  // Sort alphabetically
  Directories.Sort();
  Files.Sort();

  // Create directory items
  for (const FString &DirName : Directories) {
    if (DirName.StartsWith(TEXT("."))) {
      continue;
    }

    FString FullPath = DirectoryItem->Path / DirName;
    TSharedPtr<FFileTreeItem> Item =
        MakeShared<FFileTreeItem>(FullPath, DirName, true);
    Item->Parent = DirectoryItem;
    DirectoryItem->Children.Add(Item);
  }

  // Create file items
  for (const FString &FileName : Files) {
    if (FileName.StartsWith(TEXT("."))) {
      continue;
    }

    FString FullPath = DirectoryItem->Path / FileName;
    TSharedPtr<FFileTreeItem> Item =
        MakeShared<FFileTreeItem>(FullPath, FileName, false);
    Item->Parent = DirectoryItem;
    DirectoryItem->Children.Add(Item);
  }

  DirectoryItem->SetChildrenLoaded(true);
}

TSharedRef<ITableRow>
SFileTreeView::OnGenerateRow(TSharedPtr<FFileTreeItem> Item,
                             const TSharedRef<STableViewBase> &OwnerTable) {
  return SNew(STableRow<TSharedPtr<FFileTreeItem>>, OwnerTable)
      .Style(&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"))
      .Padding(FMargin(0.0f, 1.0f))
          [SNew(SHorizontalBox)

           // File/Folder icon (using VS Code-style SVG icons)
           +
           SHorizontalBox::Slot()
               .AutoWidth()
               .VAlign(VAlign_Center)
               .Padding(2.0f, 0.0f, 6.0f,
                        0.0f)[SNew(SImage)
                                  .Image(GetIconForItem(Item))
                                  .DesiredSizeOverride(FVector2D(16.0f, 16.0f))]

           // File/Folder name
           + SHorizontalBox::Slot().FillWidth(1.0f).VAlign(VAlign_Center)
                 [SNew(STextBlock)
                      .Text(FText::FromString(Item->DisplayName))
                      .Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
                      .ColorAndOpacity(GetTextColorForItem(Item))]];
}

void SFileTreeView::OnGetChildren(
    TSharedPtr<FFileTreeItem> Item,
    TArray<TSharedPtr<FFileTreeItem>> &OutChildren) {
  if (Item.IsValid() && Item->bIsDirectory) {
    // Lazy load children when needed
    if (!Item->HasLoadedChildren()) {
      PopulateChildren(Item);
    }
    OutChildren = Item->Children;
  }
}

void SFileTreeView::OnItemDoubleClicked(TSharedPtr<FFileTreeItem> Item) {
  if (!Item.IsValid()) {
    return;
  }

  if (Item->bIsDirectory) {
    // Toggle expansion for directories
    if (TreeView.IsValid()) {
      bool bIsExpanded = TreeView->IsItemExpanded(Item);
      TreeView->SetItemExpansion(Item, !bIsExpanded);
    }
  } else {
    // Fire callback for files
    OnFileDoubleClicked.ExecuteIfBound(Item->Path);
  }
}

void SFileTreeView::OnExpansionChanged(TSharedPtr<FFileTreeItem> Item,
                                       bool bExpanded) {
  if (Item.IsValid()) {
    Item->bIsExpanded = bExpanded;

    // If expanding and children not loaded, load them
    if (bExpanded && !Item->HasLoadedChildren()) {
      PopulateChildren(Item);
      if (TreeView.IsValid()) {
        TreeView->RequestTreeRefresh();
      }
    }
  }
}

void SFileTreeView::OnSelectionChanged(TSharedPtr<FFileTreeItem> Item,
                                       ESelectInfo::Type SelectInfo) {
  SelectedItem = Item;
}

const FSlateBrush *
SFileTreeView::GetIconForItem(TSharedPtr<FFileTreeItem> Item) const {
  const FFileTreeIconManager &IconManager = FFileTreeIconManager::Get();

  if (!Item.IsValid()) {
    // Fallback to UE default brush
    const FSlateBrush *Icon = IconManager.GetFolderIcon(false);
    return Icon ? Icon
                : FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderClosed");
  }

  if (Item->bIsDirectory) {
    // Use VS Code folder icons
    const FSlateBrush *Icon = IconManager.GetFolderIcon(Item->bIsExpanded);
    if (Icon) {
      return Icon;
    }
    // Fallback to UE default
    return Item->bIsExpanded
               ? FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen")
               : FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderClosed");
  }

  // Get VS Code file icon based on extension
  const FString Ext = Item->GetExtension();
  const FSlateBrush *Icon = IconManager.GetFileIcon(Ext);

  if (Icon) {
    return Icon;
  }

  // Fallback to UE default
  return FAppStyle::GetBrush("ContentBrowser.AssetActions.ReimportAsset");
}

// GetIconColorForItem is no longer needed as SVG icons have their own colors
FSlateColor
SFileTreeView::GetIconColorForItem(TSharedPtr<FFileTreeItem> Item) const {
  // SVG icons have built-in colors, return white to preserve them
  return FSlateColor(FLinearColor::White);
}

FSlateColor
SFileTreeView::GetTextColorForItem(TSharedPtr<FFileTreeItem> Item) const {
  if (!Item.IsValid()) {
    return FSlateColor(FileTreeColors::TextNormal);
  }

  if (SelectedItem.IsValid() && SelectedItem == Item) {
    return FSlateColor(FileTreeColors::TextSelected);
  }

  return FSlateColor(FileTreeColors::TextNormal);
}

FReply SFileTreeView::OnRefreshClicked() {
  RefreshTree();
  return FReply::Handled();
}

FReply SFileTreeView::OnCollapseAllClicked() {
  CollapseAll();
  return FReply::Handled();
}

void SFileTreeView::ExpandAll() {
  if (!TreeView.IsValid()) {
    return;
  }

  TFunction<void(const TArray<TSharedPtr<FFileTreeItem>> &)> ExpandRecursive;
  ExpandRecursive =
      [this, &ExpandRecursive](const TArray<TSharedPtr<FFileTreeItem>> &Items) {
        for (const auto &Item : Items) {
          if (Item->bIsDirectory) {
            if (!Item->HasLoadedChildren()) {
              PopulateChildren(Item);
            }
            TreeView->SetItemExpansion(Item, true);
            ExpandRecursive(Item->Children);
          }
        }
      };

  ExpandRecursive(RootItems);
  TreeView->RequestTreeRefresh();
}

void SFileTreeView::CollapseAll() {
  if (!TreeView.IsValid()) {
    return;
  }

  TFunction<void(const TArray<TSharedPtr<FFileTreeItem>> &)> CollapseRecursive;
  CollapseRecursive = [this, &CollapseRecursive](
                          const TArray<TSharedPtr<FFileTreeItem>> &Items) {
    for (const auto &Item : Items) {
      if (Item->bIsDirectory) {
        TreeView->SetItemExpansion(Item, false);
        Item->bIsExpanded = false;
        CollapseRecursive(Item->Children);
      }
    }
  };

  CollapseRecursive(RootItems);
  TreeView->RequestTreeRefresh();
}

FString SFileTreeView::GetSelectedFilePath() const {
  if (SelectedItem.IsValid() && !SelectedItem->bIsDirectory) {
    return SelectedItem->Path;
  }
  return FString();
}

#undef LOCTEXT_NAMESPACE
