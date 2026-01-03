// Copyright Yureka. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SCodeEditorTab;
class SFileTreeView;
class SBorder;
class SSplitter;

/**
 * Main IDE panel that combines the file tree and code editor
 * Provides a VSCode-like split panel experience
 */
class SIDEPanel : public SCompoundWidget {
public:
  SLATE_BEGIN_ARGS(SIDEPanel) : _DefaultRootPath(TEXT("")) {}
  /** Default root path for the file tree */
  SLATE_ARGUMENT(FString, DefaultRootPath)
  SLATE_END_ARGS()

  void Construct(const FArguments &InArgs);

  /** Set the file tree root path */
  void SetFileTreeRootPath(const FString &Path);

  /** Open a file in the code editor */
  void OpenFile(const FString &FilePath);

  /** Get the current file path */
  FString GetCurrentFilePath() const;

  /** Toggle file tree visibility */
  void ToggleFileTreeVisibility();

  /** Check if file tree is visible */
  bool IsFileTreeVisible() const { return bFileTreeVisible; }

private:
  /** Handle file double-click from file tree */
  void OnFileDoubleClicked(const FString &FilePath);

  /** Handle browse folder button */
  FReply OnBrowseFolderClicked();

  /** Handle open folder action (wrapper for delegate) */
  void OnOpenFolderClicked();

  /** Handle toggle explorer button from file tree */
  void OnToggleExplorerClicked();

  /** Handle toggle explorer button click (returns FReply) */
  FReply OnToggleExplorerButtonClicked();

  /** Update the splitter based on visibility */
  void UpdateLayout();

private:
  /** The file tree view widget */
  TSharedPtr<SFileTreeView> FileTreeView;

  /** Container for file tree (used for visibility toggle) */
  TSharedPtr<SBorder> FileTreeContainer;

  /** The code editor widget */
  TSharedPtr<SCodeEditorTab> CodeEditor;

  /** Main splitter for resizing between file tree and editor */
  TSharedPtr<SSplitter> MainSplitter;

  /** Current root path for the file tree */
  FString RootPath;

  /** Whether the file tree is visible */
  bool bFileTreeVisible = true;
};
