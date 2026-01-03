// Copyright Yureka. All Rights Reserved.

#include "SCodeEditorTab.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "SCodeEditableText.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SCodeEditorTab"

namespace EditorColors {
const FLinearColor ToolbarBackground =
    FLinearColor::FromSRGBColor(FColor::FromHex("1E1E1EFF")); // VSCode dark
const FLinearColor StatusBarBackground =
    FLinearColor::FromSRGBColor(FColor::FromHex("007ACCFF")); // Blue bar
const FLinearColor EditorBackground =
    FLinearColor::FromSRGBColor(FColor::FromHex("1E1E1EFF")); // VSCode dark
} // namespace EditorColors

void SCodeEditorTab::Construct(const FArguments &InArgs) {
  ChildSlot
      [SNew(SVerticalBox)

       // Toolbar
       + SVerticalBox::Slot().AutoHeight()[CreateToolbar()]

       // Main editor area
       +
       SVerticalBox::Slot().FillHeight(1.0f)
           [SNew(SBorder)
                .BorderBackgroundColor(EditorColors::EditorBackground)
                .BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
                .Padding(0)
                    [SAssignNew(CodeEditor, SCodeEditableText)
                         .Text(FText::FromString(TEXT(
                             "// Welcome to Pure Slate Code Editor\n// Open a "
                             "file to begin editing\n\n#include "
                             "\"CoreMinimal.h\"\n\nUCLASS()\nclass AMyActor : "
                             "public AActor\n{\n    "
                             "GENERATED_BODY()\n\npublic:\n    "
                             "UPROPERTY(EditAnywhere, BlueprintReadWrite)\n    "
                             "FString MyProperty;\n\n    "
                             "UFUNCTION(BlueprintCallable)\n    void "
                             "MyFunction()\n    {\n        UE_LOG(LogTemp, "
                             "Log, TEXT(\"Hello from Pure Slate!\"));\n    "
                             "}\n};\n")))
                         .OnTextChanged(this, &SCodeEditorTab::OnTextChanged)
                         .OnCursorMoved(this, &SCodeEditorTab::OnCursorMoved)]]

       // Status bar
       + SVerticalBox::Slot().AutoHeight()[CreateStatusBar()]];

  UE_LOG(LogTemp, Log, TEXT("InlineCodeEditor: Pure Slate editor initialized"));
}

SCodeEditorTab::~SCodeEditorTab() {
  UE_LOG(LogTemp, Log, TEXT("InlineCodeEditor: Editor destroyed"));
}

TSharedRef<SWidget> SCodeEditorTab::CreateToolbar() {
  return SNew(SBorder)
      .BorderBackgroundColor(EditorColors::ToolbarBackground)
      .BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
      .Padding(FMargin(
          8,
          6))[SNew(SHorizontalBox)

              // New File
              + SHorizontalBox::Slot().AutoWidth().Padding(
                    0, 0, 4,
                    0)[SNew(SButton)
                           .Text(LOCTEXT("NewFile", "New"))
                           .ToolTipText(
                               LOCTEXT("NewFileTooltip", "Create a new file"))
                           .OnClicked(this, &SCodeEditorTab::OnNewFileClicked)]

              // Save
              + SHorizontalBox::Slot().AutoWidth().Padding(
                    0, 0, 4,
                    0)[SNew(SButton)
                           .Text(LOCTEXT("Save", "Save"))
                           .ToolTipText(LOCTEXT("SaveTooltip",
                                                "Save current file (Ctrl+S)"))
                           .OnClicked(this, &SCodeEditorTab::OnSaveClicked)]

              // Separator
              + SHorizontalBox::Slot().AutoWidth().Padding(
                    8, 0)[SNew(SBox).WidthOverride(
                    1)[SNew(SBorder).BorderBackgroundColor(
                    FLinearColor(0.3f, 0.3f, 0.3f))]]

              // Go to line label
              + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    .Padding(
                        0, 0, 4,
                        0)[SNew(STextBlock)
                               .Text(LOCTEXT("GoToLine", "Go to line:"))
                               .ColorAndOpacity(FLinearColor(0.8f, 0.8f, 0.8f))]

              // Go to line input
              + SHorizontalBox::Slot().AutoWidth().Padding(
                    0, 0, 4, 0)[SNew(SBox).WidthOverride(
                    60)[SAssignNew(GoToLineInput, SEditableTextBox)
                            .HintText(LOCTEXT("LineNumber", "Line #"))
                            .OnTextCommitted_Lambda(
                                [this](const FText &Text,
                                       ETextCommit::Type CommitType) {
                                  if (CommitType == ETextCommit::OnEnter) {
                                    int32 LineNum =
                                        FCString::Atoi(*Text.ToString());
                                    if (CodeEditor.IsValid() && LineNum > 0) {
                                      CodeEditor->GoToLine(LineNum);
                                      CodeEditor->FocusEditor();
                                    }
                                  }
                                })]]

              // Spacer
              +
              SHorizontalBox::Slot().FillWidth(1.0f)[SNullWidget::NullWidget]];
}

TSharedRef<SWidget> SCodeEditorTab::CreateStatusBar() {
  return SNew(SBorder)
      .BorderBackgroundColor(EditorColors::StatusBarBackground)
      .BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
      .Padding(FMargin(
          12,
          4))[SNew(SHorizontalBox)

              // Line/Column
              + SHorizontalBox::Slot().AutoWidth().Padding(
                    0, 0, 16, 0)[SAssignNew(StatusLineColumn, STextBlock)
                                     .Text(LOCTEXT("LineCol", "Ln 1, Col 1"))
                                     .ColorAndOpacity(FLinearColor::White)]

              // Language
              + SHorizontalBox::Slot().AutoWidth().Padding(
                    0, 0, 16, 0)[SAssignNew(StatusLanguage, STextBlock)
                                     .Text(LOCTEXT("Language", "C++"))
                                     .ColorAndOpacity(FLinearColor::White)]

              // Spacer
              + SHorizontalBox::Slot().FillWidth(1.0f)[SNullWidget::NullWidget]

              // Status message
              + SHorizontalBox::Slot()
                    .AutoWidth()[SAssignNew(StatusMessage, STextBlock)
                                     .Text(LOCTEXT("Ready", "Ready"))
                                     .ColorAndOpacity(FLinearColor::White)]];
}

void SCodeEditorTab::OpenFile(const FString &FilePath) {
  if (!FPaths::FileExists(FilePath)) {
    UE_LOG(LogTemp, Warning, TEXT("InlineCodeEditor: File not found: %s"),
           *FilePath);
    return;
  }

  FString FileContent;
  if (!FFileHelper::LoadFileToString(FileContent, *FilePath)) {
    UE_LOG(LogTemp, Error, TEXT("InlineCodeEditor: Failed to load file: %s"),
           *FilePath);
    return;
  }

  CurrentFilePath = FilePath;

  if (CodeEditor.IsValid()) {
    CodeEditor->SetPlainText(FileContent);
    CodeEditor->SetFilePath(FilePath);
    CodeEditor->ClearModified();
    CodeEditor->FocusEditor();
  }

  bHasUnsavedChanges = false;

  // Update status
  if (StatusMessage.IsValid()) {
    FString FileName = FPaths::GetCleanFilename(FilePath);
    StatusMessage->SetText(FText::Format(LOCTEXT("OpenedFile", "Opened: {0}"),
                                         FText::FromString(FileName)));
  }

  // Update language display
  if (StatusLanguage.IsValid()) {
    FString Ext = FPaths::GetExtension(FilePath).ToLower();
    if (Ext == TEXT("cpp") || Ext == TEXT("cc") || Ext == TEXT("cxx")) {
      StatusLanguage->SetText(LOCTEXT("LangCpp", "C++"));
    } else if (Ext == TEXT("h") || Ext == TEXT("hpp")) {
      StatusLanguage->SetText(LOCTEXT("LangHeader", "C++ Header"));
    } else if (Ext == TEXT("c")) {
      StatusLanguage->SetText(LOCTEXT("LangC", "C"));
    } else {
      StatusLanguage->SetText(LOCTEXT("LangPlain", "Plain Text"));
    }
  }

  UE_LOG(LogTemp, Log, TEXT("InlineCodeEditor: Opened %s"), *FilePath);
}

bool SCodeEditorTab::SaveFile() {
  if (CurrentFilePath.IsEmpty()) {
    if (StatusMessage.IsValid()) {
      StatusMessage->SetText(LOCTEXT("NoFile", "No file to save"));
    }
    return false;
  }

  if (!CodeEditor.IsValid()) {
    return false;
  }

  FString Content = CodeEditor->GetPlainText();

  if (!FFileHelper::SaveStringToFile(Content, *CurrentFilePath)) {
    UE_LOG(LogTemp, Error, TEXT("InlineCodeEditor: Failed to save file: %s"),
           *CurrentFilePath);
    if (StatusMessage.IsValid()) {
      StatusMessage->SetText(LOCTEXT("SaveFailed", "Save failed!"));
    }
    return false;
  }

  CodeEditor->ClearModified();
  bHasUnsavedChanges = false;

  if (StatusMessage.IsValid()) {
    FString FileName = FPaths::GetCleanFilename(CurrentFilePath);
    StatusMessage->SetText(FText::Format(LOCTEXT("SavedFile", "Saved: {0}"),
                                         FText::FromString(FileName)));
  }

  UE_LOG(LogTemp, Log, TEXT("InlineCodeEditor: Saved %s"), *CurrentFilePath);
  return true;
}

bool SCodeEditorTab::HasUnsavedChanges() const {
  if (CodeEditor.IsValid()) {
    return CodeEditor->IsModified();
  }
  return bHasUnsavedChanges;
}

FReply SCodeEditorTab::OnNewFileClicked() {
  CurrentFilePath.Empty();

  if (CodeEditor.IsValid()) {
    CodeEditor->SetPlainText(TEXT("// New file\n\n"));
    CodeEditor->ClearModified();
    CodeEditor->FocusEditor();
  }

  bHasUnsavedChanges = false;

  if (StatusMessage.IsValid()) {
    StatusMessage->SetText(LOCTEXT("NewFileCreated", "New file created"));
  }

  return FReply::Handled();
}

FReply SCodeEditorTab::OnSaveClicked() {
  SaveFile();
  return FReply::Handled();
}

void SCodeEditorTab::OnTextChanged(const FText &NewText) {
  bHasUnsavedChanges = true;
}

void SCodeEditorTab::OnCursorMoved() { UpdateStatusBar(); }

void SCodeEditorTab::UpdateStatusBar() {
  if (!CodeEditor.IsValid() || !StatusLineColumn.IsValid()) {
    return;
  }

  int32 Line = CodeEditor->GetCursorLine();
  int32 Column = CodeEditor->GetCursorColumn();

  StatusLineColumn->SetText(
      FText::Format(LOCTEXT("LineColFormat", "Ln {0}, Col {1}"),
                    FText::AsNumber(Line), FText::AsNumber(Column)));
}

#undef LOCTEXT_NAMESPACE
