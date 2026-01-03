// Copyright Yureka. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SCompoundWidget.h"

class SCodeEditableText;
class SEditableTextBox;
class STextBlock;

/**
 * Inline code editor tab using native Slate widgets
 * Features: Toolbar, code editor with line numbers, status bar
 */
class SCodeEditorTab : public SCompoundWidget {
public:
  SLATE_BEGIN_ARGS(SCodeEditorTab) {}
  SLATE_END_ARGS()

  void Construct(const FArguments &InArgs);
  virtual ~SCodeEditorTab() override;

  /** Set the parent dock tab for visibility tracking */
  void SetParentTab(TSharedPtr<SDockTab> InTab) { ParentTab = InTab; }

  /** Open a file in the editor */
  void OpenFile(const FString &FilePath);

  /** Get the current file path */
  FString GetCurrentFilePath() const { return CurrentFilePath; }

  /** Save the current file */
  bool SaveFile();

  /** Check if current file has unsaved changes */
  bool HasUnsavedChanges() const;

private:
  /** Create the toolbar widget */
  TSharedRef<SWidget> CreateToolbar();

  /** Create the status bar widget */
  TSharedRef<SWidget> CreateStatusBar();

  /** Handle new file button */
  FReply OnNewFileClicked();

  /** Handle save button */
  FReply OnSaveClicked();

  /** Handle text changed in editor */
  void OnTextChanged(const FText &NewText);

  /** Handle cursor position changed */
  void OnCursorMoved();

  /** Update status bar text */
  void UpdateStatusBar();

private:
  /** The code editor widget */
  TSharedPtr<SCodeEditableText> CodeEditor;

  /** Parent dock tab reference */
  TWeakPtr<SDockTab> ParentTab;

  /** Currently open file path */
  FString CurrentFilePath;

  /** Status bar line/column text */
  TSharedPtr<STextBlock> StatusLineColumn;

  /** Status bar language text */
  TSharedPtr<STextBlock> StatusLanguage;

  /** Status bar message text */
  TSharedPtr<STextBlock> StatusMessage;

  /** Go to line input box */
  TSharedPtr<SEditableTextBox> GoToLineInput;

  /** Whether there are unsaved changes */
  bool bHasUnsavedChanges = false;
};
