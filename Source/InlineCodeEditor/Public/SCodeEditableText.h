// Copyright InlineEditor Team. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Text/SMultiLineEditableText.h"

class FCppSyntaxHighlighter;
class SScrollBar;
class SVerticalBox;

/**
 * Represents a foldable code region (e.g., function body, class, etc.)
 */
struct FCodeFoldRegion {
  int32 StartLine = 0;
  int32 EndLine = 0;
  bool bIsFolded = false;
  int32 IndentLevel = 0;

  FCodeFoldRegion() = default;
  FCodeFoldRegion(int32 InStart, int32 InEnd, int32 InIndent = 0)
      : StartLine(InStart), EndLine(InEnd), IndentLevel(InIndent) {}
};

/**
 * Widget that draws indentation guide lines (VS Code style)
 * Draws vertical lines at each indent level for each line based on
 * the line's leading whitespace.
 */
class SIndentGuides : public SLeafWidget {
public:
  SLATE_BEGIN_ARGS(SIndentGuides) {}
  SLATE_ARGUMENT(float, LineHeight)
  SLATE_ARGUMENT(float, CharWidth)
  SLATE_ARGUMENT(int32, IndentSize)
  SLATE_END_ARGS()

  void Construct(const FArguments &InArgs);

  virtual int32 OnPaint(const FPaintArgs &Args,
                        const FGeometry &AllottedGeometry,
                        const FSlateRect &MyCullingRect,
                        FSlateWindowElementList &OutDrawElements, int32 LayerId,
                        const FWidgetStyle &InWidgetStyle,
                        bool bParentEnabled) const override;

  virtual FVector2D ComputeDesiredSize(float) const override;

  /** Set the indent level for each line */
  void SetLineIndentLevels(const TArray<int32> &InLevels);
  void SetLineHeight(float InLineHeight) { LineHeight = InLineHeight; }
  void SetCharWidth(float InCharWidth) { CharWidth = InCharWidth; }

private:
  /** Indent level for each line (how many guides to draw) */
  TArray<int32> LineIndentLevels;

  float LineHeight = 15.0f;
  float CharWidth = 8.0f;
  int32 IndentSize = 2; // Number of spaces per indent level
  FLinearColor GuideColor;
};

/**
 * A code editor widget with:
 * - Code folding
 * - Character-based indentation guides
 * - Monaco Dark+ theme styling
 * - Syntax highlighting
 */
class INLINECODEEDITOR_API SCodeEditableText : public SCompoundWidget {
public:
  SLATE_BEGIN_ARGS(SCodeEditableText)
      : _Text(), _IsReadOnly(false), _FilePath() {}
  SLATE_ATTRIBUTE(FText, Text)
  SLATE_ARGUMENT(bool, IsReadOnly)
  SLATE_ARGUMENT(FString, FilePath)
  SLATE_EVENT(FOnTextChanged, OnTextChanged)
  SLATE_EVENT(FSimpleDelegate, OnCursorMoved)
  SLATE_END_ARGS()

  void Construct(const FArguments &InArgs);

  FText GetText() const;
  void SetText(const FText &InText);
  FString GetPlainText() const;
  void SetPlainText(const FString &InText);

  int32 GetCursorLine() const { return CurrentLine; }
  int32 GetCursorColumn() const { return CurrentColumn; }
  void GoToLine(int32 LineNumber);
  int32 GetLineCount() const { return TotalLines; }

  void SetFilePath(const FString &InPath) { FilePath = InPath; }
  FString GetFilePath() const { return FilePath; }

  bool IsModified() const { return bIsModified; }
  void ClearModified() { bIsModified = false; }

  void FocusEditor();
  void ToggleFoldAtLine(int32 LineNumber);
  void FoldAll();
  void UnfoldAll();

private:
  void HandleTextChanged(const FText &NewText);
  void HandleCursorMoved(const FTextLocation &NewLocation);
  void RebuildFoldingGutter();
  void ParseFoldRegions();
  void ParseLines();

  /** Calculate indent levels for each line based on leading whitespace */
  void CalculateLineIndentLevels();

  void ApplyFolding();
  FCodeFoldRegion *GetFoldRegionAtLine(int32 LineIndex);
  bool IsLineHidden(int32 LineIndex) const;
  FReply OnFoldButtonClicked(int32 LineIndex);
  void UpdateIndentGuides();

private:
  TSharedPtr<SMultiLineEditableText> TextEditor;
  TSharedPtr<SVerticalBox> FoldingGutter;
  TSharedPtr<class SScrollBox> EditorScrollBox;
  TSharedPtr<SIndentGuides> IndentGuidesWidget;
  TSharedPtr<FCppSyntaxHighlighter> SyntaxMarshaller;

  TArray<FCodeFoldRegion> FoldRegions;

  /** Indent level for each displayed line */
  TArray<int32> LineIndentLevels;

  FString OriginalText;
  FString DisplayedText;
  TArray<int32> DisplayToOriginalLine;

  FString FilePath;
  bool bIsModified = false;
  int32 CurrentLine = 1;
  int32 CurrentColumn = 1;
  int32 TotalLines = 1;

  /** Measured character width in pixels */
  float CharacterWidth = 8.0f;

  /** Indent size in spaces */
  static constexpr int32 IndentSize = 2;

  FOnTextChanged OnTextChangedCallback;
  FSimpleDelegate OnCursorMovedCallback;
  bool bIsUpdatingText = false;
};
