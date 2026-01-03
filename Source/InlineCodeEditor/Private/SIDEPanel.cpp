// Copyright Yureka. All Rights Reserved.

#include "SIDEPanel.h"
#include "DesktopPlatformModule.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/Paths.h"
#include "SCodeEditorTab.h"
#include "SFileTreeView.h"
#include "Styling/AppStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SIDEPanel"

// Use EXACT same color method as code editor - FromSRGBColor for correct
// matching
namespace IDEColors {
const FLinearColor Background =
    FLinearColor::FromSRGBColor(FColor::FromHex("1E1E1EFF")); // VSCode dark
const FLinearColor IconColor =
    FLinearColor(0.8f, 0.8f, 0.8f); // #CCCCCC - normal text color
} // namespace IDEColors

void SIDEPanel::Construct(const FArguments &InArgs) {
  RootPath = InArgs._DefaultRootPath;

  // If no path specified, use the project directory
  if (RootPath.IsEmpty()) {
    RootPath = FPaths::ProjectDir();
  }

  ChildSlot
      [SNew(SBorder)
           .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
           .BorderBackgroundColor(IDEColors::Background)
           .Padding(0)
               [SNew(SHorizontalBox)

                // Activity bar (always visible, contains toggle button)
                +
                SHorizontalBox::Slot().AutoWidth()
                    [SNew(SBorder)
                         .BorderImage(FAppStyle::GetBrush("WhiteBrush"))
                         .BorderBackgroundColor(IDEColors::Background)
                         .Padding(FMargin(2.0f, 4.0f))
                             [SNew(SVerticalBox)

                              // Toggle explorer button (always visible)
                              + SVerticalBox::Slot().AutoHeight().Padding(0.0f,
                                                                          2.0f)
                                    [SNew(SButton)
                                         .ButtonStyle(FAppStyle::Get(), "FlatBu"
                                                                        "tton")
                                         .ContentPadding(FMargin(4.0f))
                                         .OnClicked(
                                             this,
                                             &SIDEPanel::
                                                 OnToggleExplorerButtonClicked)
                                         .ToolTipText(LOCTEXT(
                                             "ToggleExplorerTooltip",
                                             "Toggle Explorer (Ctrl+B)"))
                                             [SNew(SImage)
                                                  .Image(FAppStyle::GetBrush(
                                                      "ContentBrowser."
                                                      "AssetTreeFolderOpen"))
                                                  .ColorAndOpacity(FSlateColor(
                                                      IDEColors::IconColor))
                                                  .DesiredSizeOverride(
                                                      FVector2D(20.0f, 20.0f))]]

                              // Open folder button (always visible)
                              +
                              SVerticalBox::Slot().AutoHeight().Padding(0.0f,
                                                                        2.0f)
                                  [SNew(SButton)
                                       .ButtonStyle(FAppStyle::Get(), "FlatButt"
                                                                      "on")
                                       .ContentPadding(FMargin(4.0f))
                                       .OnClicked(
                                           this,
                                           &SIDEPanel::OnBrowseFolderClicked)
                                       .ToolTipText(LOCTEXT("OpenFolderTooltip",
                                                            "Open Folder"))
                                           [SNew(SImage)
                                                .Image(FAppStyle::GetBrush(
                                                    "Icons.FolderOpen"))
                                                .ColorAndOpacity(FSlateColor(
                                                    IDEColors::IconColor))
                                                .DesiredSizeOverride(
                                                    FVector2D(20.0f, 20.0f))]]]]

                // Main content with RESIZABLE splitter
                + SHorizontalBox::Slot().FillWidth(1.0f)
                      [SAssignNew(MainSplitter, SSplitter)
                           .Orientation(Orient_Horizontal)
                           .PhysicalSplitterHandleSize(2.0f)
                           .HitDetectionSplitterHandleSize(6.0f)
                           .Style(FAppStyle::Get(), "SplitterDark")

                       // File tree panel (can be toggled)
                       + SSplitter::Slot().Value(0.25f).MinSize(150.0f)
                             [SAssignNew(FileTreeContainer, SBorder)
                                  .BorderImage(FAppStyle::GetBrush("NoBorder"))
                                  .Padding(
                                      0)[SAssignNew(FileTreeView, SFileTreeView)
                                             .RootPath(RootPath)
                                             .OnFileDoubleClicked(
                                                 FOnFileSelected::CreateSP(
                                                     this,
                                                     &SIDEPanel::
                                                         OnFileDoubleClicked))]]

                       // Code editor panel (fills remaining space)
                       + SSplitter::Slot().Value(
                             0.75f)[SAssignNew(CodeEditor, SCodeEditorTab)]]]];
}

void SIDEPanel::SetFileTreeRootPath(const FString &Path) {
  RootPath = Path;
  if (FileTreeView.IsValid()) {
    FileTreeView->SetRootPath(Path);
  }
}

void SIDEPanel::OpenFile(const FString &FilePath) {
  if (CodeEditor.IsValid()) {
    CodeEditor->OpenFile(FilePath);
  }
}

FString SIDEPanel::GetCurrentFilePath() const {
  if (CodeEditor.IsValid()) {
    return CodeEditor->GetCurrentFilePath();
  }
  return FString();
}

void SIDEPanel::ToggleFileTreeVisibility() {
  bFileTreeVisible = !bFileTreeVisible;

  if (FileTreeContainer.IsValid()) {
    FileTreeContainer->SetVisibility(bFileTreeVisible ? EVisibility::Visible
                                                      : EVisibility::Collapsed);
  }
}

void SIDEPanel::OnFileDoubleClicked(const FString &FilePath) {
  OpenFile(FilePath);
}

void SIDEPanel::OnToggleExplorerClicked() { ToggleFileTreeVisibility(); }

FReply SIDEPanel::OnToggleExplorerButtonClicked() {
  ToggleFileTreeVisibility();
  return FReply::Handled();
}

void SIDEPanel::OnOpenFolderClicked() { OnBrowseFolderClicked(); }

FReply SIDEPanel::OnBrowseFolderClicked() {
  IDesktopPlatform *DesktopPlatform = FDesktopPlatformModule::Get();
  if (DesktopPlatform) {
    FString SelectedPath;
    const void *ParentWindowHandle =
        FSlateApplication::Get().GetActiveTopLevelWindow().IsValid()
            ? FSlateApplication::Get()
                  .GetActiveTopLevelWindow()
                  ->GetNativeWindow()
                  ->GetOSWindowHandle()
            : nullptr;

    if (DesktopPlatform->OpenDirectoryDialog(
            ParentWindowHandle,
            LOCTEXT("SelectFolder", "Select Folder to Open").ToString(),
            RootPath, SelectedPath)) {
      SetFileTreeRootPath(SelectedPath);
    }
  }

  return FReply::Handled();
}

void SIDEPanel::UpdateLayout() {
  // Reserved for future layout updates
}

#undef LOCTEXT_NAMESPACE
