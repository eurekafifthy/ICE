// Copyright Yureka. All Rights Reserved.

#include "SCodeEditableText.h"
#include "FCppSyntaxHighlighter.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"
#include "Rendering/DrawElements.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SCodeEditableText"

namespace CodeEditorStyle {
constexpr float FoldGutterWidth = 16.0f;

const FLinearColor BackgroundColor =
    FLinearColor::FromSRGBColor(FColor::FromHex("1E1E1EFF")); // VSCode dark

const FLinearColor GutterColor =
    FLinearColor::FromSRGBColor(FColor::FromHex("1E1E1EFF")); // VSCode dark

const FLinearColor FoldIndicatorColor =
    FLinearColor::FromSRGBColor(FColor::FromHex("808080FF"));

// Indent guide color (subtle, like VS Code)
const FLinearColor IndentGuideColor =
    FLinearColor::FromSRGBColor(FColor::FromHex("404040FF"));

const FLinearColor TextColor =
    FLinearColor::FromSRGBColor(FColor::FromHex("D4D4D4FF"));

constexpr int32 FontSize = 10;
constexpr float LineHeight = 15.0f;
} // namespace CodeEditorStyle

//////////////////////////////////////////////////////////////////////////
// SIndentGuides - Character-based indent guides (VS Code style)

void SIndentGuides::Construct(const FArguments &InArgs) {
  LineHeight = InArgs._LineHeight;
  CharWidth = InArgs._CharWidth;
  IndentSize = InArgs._IndentSize;
  GuideColor = CodeEditorStyle::IndentGuideColor;
}

FVector2D SIndentGuides::ComputeDesiredSize(float) const {
  float TotalHeight = LineIndentLevels.Num() * LineHeight;

  int32 MaxIndent = 0;
  for (int32 Level : LineIndentLevels) {
    MaxIndent = FMath::Max(MaxIndent, Level);
  }
  float TotalWidth = MaxIndent * IndentSize * CharWidth;

  return FVector2D(TotalWidth, TotalHeight);
}

void SIndentGuides::SetLineIndentLevels(const TArray<int32> &InLevels) {
  LineIndentLevels = InLevels;
  Invalidate(EInvalidateWidgetReason::Paint);
}

int32 SIndentGuides::OnPaint(const FPaintArgs &Args,
                             const FGeometry &AllottedGeometry,
                             const FSlateRect &MyCullingRect,
                             FSlateWindowElementList &OutDrawElements,
                             int32 LayerId, const FWidgetStyle &InWidgetStyle,
                             bool bParentEnabled) const {
  if (LineIndentLevels.Num() == 0 || CharWidth <= 0.0f) {
    return LayerId;
  }

  const FSlateBrush *WhiteBrush = FCoreStyle::Get().GetBrush("WhiteBrush");

  // For each line, draw vertical guides at each indent level
  for (int32 LineIdx = 0; LineIdx < LineIndentLevels.Num(); ++LineIdx) {
    int32 IndentLevel = LineIndentLevels[LineIdx];
    float LineY = LineIdx * LineHeight;

    // Draw a vertical line at each indent level from 1 to IndentLevel
    // The guide at level N is drawn at column (N-1) * IndentSize
    for (int32 Level = 1; Level <= IndentLevel; ++Level) {
      // Column position: (Level - 1) * IndentSize gives us the character column
      float X = (Level - 1) * IndentSize * CharWidth;

      // Draw a 1px vertical line for this line's height
      FSlateDrawElement::MakeBox(
          OutDrawElements, LayerId,
          AllottedGeometry.ToPaintGeometry(
              FVector2D(1.0f, LineHeight),
              FSlateLayoutTransform(FVector2D(X, LineY))),
          WhiteBrush, ESlateDrawEffect::None, GuideColor);
    }
  }

  return LayerId + 1;
}

//////////////////////////////////////////////////////////////////////////
// SCodeEditableText Implementation

void SCodeEditableText::Construct(const FArguments &InArgs) {
  FilePath = InArgs._FilePath;
  OnTextChangedCallback = InArgs._OnTextChanged;
  OnCursorMovedCallback = InArgs._OnCursorMoved;

  SyntaxMarshaller = FCppSyntaxHighlighter::Create();

  FText InitialText = InArgs._Text.Get();
  OriginalText = InitialText.ToString();

  // Measure character width
  FSlateFontInfo MonoFont =
      FCoreStyle::GetDefaultFontStyle("Mono", CodeEditorStyle::FontSize);
  TSharedRef<FSlateFontMeasure> FontMeasure =
      FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
  CharacterWidth = FontMeasure->Measure(TEXT(" "), MonoFont).X;

  FTextBlockStyle EditorTextStyle =
      FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
  EditorTextStyle.SetFont(MonoFont);
  EditorTextStyle.SetColorAndOpacity(FSlateColor(CodeEditorStyle::TextColor));

  TSharedPtr<SScrollBar> SharedVScrollBar;

  ChildSlot
      [SNew(SHorizontalBox)

       +
       SHorizontalBox::Slot().FillWidth(1.0f)
           [SNew(SBorder)
                .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                .BorderBackgroundColor(CodeEditorStyle::BackgroundColor)
                .Padding(FMargin(0))
                    [SAssignNew(EditorScrollBox, SScrollBox)
                         .Orientation(Orient_Vertical)
                         .ExternalScrollbar(SharedVScrollBar)
                         .ConsumeMouseWheel(EConsumeMouseWheel::Always)

                     + SScrollBox::Slot()
                           [SNew(SHorizontalBox)

                            // Folding gutter
                            + SHorizontalBox::Slot().AutoWidth().Padding(
                                  FMargin(4, 4, 0, 4))[SNew(SBox).WidthOverride(
                                  CodeEditorStyle::FoldGutterWidth)[SAssignNew(
                                  FoldingGutter, SVerticalBox)]]

                            // Text editor with indent guides overlay
                            + SHorizontalBox::Slot().FillWidth(1.0f).Padding(
                                  FMargin(0, 4, 0, 4))
                                  [SNew(SOverlay)

                                   // Layer 0: Indent guides (behind text)
                                   + SOverlay::Slot()
                                         [SAssignNew(IndentGuidesWidget,
                                                     SIndentGuides)
                                              .LineHeight(
                                                  CodeEditorStyle::LineHeight)
                                              .CharWidth(CharacterWidth)
                                              .IndentSize(IndentSize)]

                                   // Layer 1: Text editor (on top)
                                   + SOverlay::Slot()
                                         [SAssignNew(TextEditor,
                                                     SMultiLineEditableText)
                                              .Text(InitialText)
                                              .TextStyle(&EditorTextStyle)
                                              .Marshaller(SyntaxMarshaller)
                                              .IsReadOnly(InArgs._IsReadOnly)
                                              .AutoWrapText(false)
                                              .Margin(FMargin(0))
                                              .OnTextChanged(
                                                  this, &SCodeEditableText::
                                                            HandleTextChanged)
                                              .OnCursorMoved(
                                                  this,
                                                  &SCodeEditableText::
                                                      HandleCursorMoved)]]]]]

       + SHorizontalBox::Slot()
             .AutoWidth()[SAssignNew(SharedVScrollBar, SScrollBar)
                              .Orientation(Orient_Vertical)
                              .Thickness(FVector2D(8.0f, 8.0f))]];

  ParseLines();
  ParseFoldRegions();
  CalculateLineIndentLevels();
  RebuildFoldingGutter();
  UpdateIndentGuides();
}

void SCodeEditableText::CalculateLineIndentLevels() {
  LineIndentLevels.Empty();

  TArray<FString> Lines;
  (DisplayedText.IsEmpty() ? OriginalText : DisplayedText)
      .ParseIntoArray(Lines, TEXT("\n"), false);

  for (const FString &Line : Lines) {
    // Count leading whitespace
    int32 SpaceCount = 0;
    for (int32 i = 0; i < Line.Len(); ++i) {
      TCHAR C = Line[i];
      if (C == ' ') {
        SpaceCount++;
      } else if (C == '\t') {
        // Tab aligns to next tab stop
        SpaceCount += IndentSize - (SpaceCount % IndentSize);
      } else {
        // First non-whitespace character
        break;
      }
    }

    // Convert to indent level
    // If line is empty or whitespace-only, use 0
    bool bIsEmptyLine = true;
    for (TCHAR C : Line) {
      if (!FChar::IsWhitespace(C)) {
        bIsEmptyLine = false;
        break;
      }
    }

    int32 IndentLevel = bIsEmptyLine ? 0 : (SpaceCount / IndentSize);
    LineIndentLevels.Add(IndentLevel);
  }

  // VS Code behavior: for empty/whitespace-only lines, inherit indent from
  // context Look at surrounding non-empty lines to determine indent level
  for (int32 i = 0; i < LineIndentLevels.Num(); ++i) {
    if (LineIndentLevels[i] == 0) {
      // Check if the line is truly empty
      const FString &Line = Lines[i];
      bool bIsEmpty = true;
      for (TCHAR C : Line) {
        if (!FChar::IsWhitespace(C)) {
          bIsEmpty = false;
          break;
        }
      }

      if (bIsEmpty && i > 0) {
        // Look for the minimum of previous and next non-empty lines
        int32 PrevIndent = 0;
        for (int32 j = i - 1; j >= 0; --j) {
          if (LineIndentLevels[j] > 0 ||
              !Lines[j].TrimStartAndEnd().IsEmpty()) {
            PrevIndent = LineIndentLevels[j];
            break;
          }
        }

        int32 NextIndent = 0;
        for (int32 j = i + 1; j < LineIndentLevels.Num(); ++j) {
          if (LineIndentLevels[j] > 0 ||
              !Lines[j].TrimStartAndEnd().IsEmpty()) {
            NextIndent = LineIndentLevels[j];
            break;
          }
        }

        // Use the minimum of the two (or just prev if next is 0)
        if (NextIndent > 0 && PrevIndent > 0) {
          LineIndentLevels[i] = FMath::Min(PrevIndent, NextIndent);
        } else if (PrevIndent > 0) {
          LineIndentLevels[i] = PrevIndent;
        } else if (NextIndent > 0) {
          LineIndentLevels[i] = NextIndent;
        }
      }
    }
  }
}

void SCodeEditableText::UpdateIndentGuides() {
  if (IndentGuidesWidget.IsValid()) {
    IndentGuidesWidget->SetLineIndentLevels(LineIndentLevels);
  }
}

void SCodeEditableText::RebuildFoldingGutter() {
  if (!FoldingGutter.IsValid()) {
    return;
  }

  FoldingGutter->ClearChildren();

  const FSlateFontInfo FontInfo =
      FCoreStyle::GetDefaultFontStyle("Mono", CodeEditorStyle::FontSize);

  TArray<FString> Lines;
  OriginalText.ParseIntoArray(Lines, TEXT("\n"), false);

  for (int32 i = 0; i < Lines.Num(); ++i) {
    if (IsLineHidden(i)) {
      continue;
    }

    FCodeFoldRegion *Region = GetFoldRegionAtLine(i);

    if (Region != nullptr) {
      FText ButtonText = Region->bIsFolded ? FText::FromString(TEXT("\u25B6"))
                                           : FText::FromString(TEXT("\u25BC"));

      int32 LineIndex = i;
      FoldingGutter->AddSlot()
          .AutoHeight()
          .HAlign(HAlign_Center)
          .VAlign(VAlign_Center)
              [SNew(SBox)
                   .HeightOverride(CodeEditorStyle::LineHeight)
                   .WidthOverride(CodeEditorStyle::FoldGutterWidth)
                       [SNew(SButton)
                            .ButtonStyle(FCoreStyle::Get(), "NoBorder")
                            .ContentPadding(FMargin(0))
                            .Cursor(EMouseCursor::Hand)
                            .OnClicked_Lambda([this, LineIndex]() {
                              return OnFoldButtonClicked(LineIndex);
                            })[SNew(STextBlock)
                                   .Text(ButtonText)
                                   .Font(FontInfo)
                                   .Justification(ETextJustify::Center)
                                   .ColorAndOpacity(
                                       CodeEditorStyle::FoldIndicatorColor)]]];
    } else {
      FoldingGutter->AddSlot()
          .AutoHeight()
          .HAlign(HAlign_Center)
          .VAlign(
              VAlign_Center)[SNew(SBox)
                                 .HeightOverride(CodeEditorStyle::LineHeight)
                                 .WidthOverride(
                                     CodeEditorStyle::FoldGutterWidth)];
    }
  }
}

void SCodeEditableText::ParseFoldRegions() {
  FoldRegions.Empty();

  TArray<FString> Lines;
  OriginalText.ParseIntoArray(Lines, TEXT("\n"), false);

  TArray<int32> BraceStack;

  for (int32 i = 0; i < Lines.Num(); ++i) {
    const FString &Line = Lines[i];

    bool bInString = false;
    bool bInChar = false;
    bool bInLineComment = false;

    for (int32 j = 0; j < Line.Len(); ++j) {
      TCHAR C = Line[j];
      TCHAR NextC = (j + 1 < Line.Len()) ? Line[j + 1] : 0;
      TCHAR PrevC = (j > 0) ? Line[j - 1] : 0;

      if (PrevC == '\\') {
        continue;
      }

      if (C == '"' && !bInLineComment && !bInChar) {
        bInString = !bInString;
        continue;
      }

      if (C == '\'' && !bInLineComment && !bInString) {
        bInChar = !bInChar;
        continue;
      }

      if (C == '/' && NextC == '/' && !bInString && !bInChar) {
        bInLineComment = true;
        continue;
      }

      if (bInString || bInChar || bInLineComment) {
        continue;
      }

      if (C == '{') {
        BraceStack.Push(i);
      } else if (C == '}') {
        if (BraceStack.Num() > 0) {
          int32 StartLine = BraceStack.Pop();
          if (i > StartLine) {
            FoldRegions.Add(FCodeFoldRegion(StartLine, i, BraceStack.Num()));
          }
        }
      }
    }
  }

  FoldRegions.Sort([](const FCodeFoldRegion &A, const FCodeFoldRegion &B) {
    return A.StartLine < B.StartLine;
  });
}

FCodeFoldRegion *SCodeEditableText::GetFoldRegionAtLine(int32 LineIndex) {
  for (FCodeFoldRegion &Region : FoldRegions) {
    if (Region.StartLine == LineIndex) {
      return &Region;
    }
  }
  return nullptr;
}

bool SCodeEditableText::IsLineHidden(int32 LineIndex) const {
  for (const FCodeFoldRegion &Region : FoldRegions) {
    if (Region.bIsFolded && LineIndex > Region.StartLine &&
        LineIndex <= Region.EndLine) {
      return true;
    }
  }
  return false;
}

FReply SCodeEditableText::OnFoldButtonClicked(int32 LineIndex) {
  FCodeFoldRegion *Region = GetFoldRegionAtLine(LineIndex);
  if (Region) {
    Region->bIsFolded = !Region->bIsFolded;
    ApplyFolding();
    RebuildFoldingGutter();
  }
  return FReply::Handled();
}

void SCodeEditableText::ApplyFolding() {
  if (!TextEditor.IsValid()) {
    return;
  }

  bIsUpdatingText = true;

  TArray<FString> Lines;
  OriginalText.ParseIntoArray(Lines, TEXT("\n"), false);

  DisplayToOriginalLine.Empty();
  TArray<FString> VisibleLines;

  for (int32 i = 0; i < Lines.Num(); ++i) {
    if (IsLineHidden(i)) {
      continue;
    }

    FCodeFoldRegion *Region = GetFoldRegionAtLine(i);
    if (Region && Region->bIsFolded) {
      FString FoldedLine = Lines[i] + TEXT(" ... }");
      VisibleLines.Add(FoldedLine);
    } else {
      VisibleLines.Add(Lines[i]);
    }
    DisplayToOriginalLine.Add(i);
  }

  DisplayedText = FString::Join(VisibleLines, TEXT("\n"));
  TextEditor->SetText(FText::FromString(DisplayedText));

  CalculateLineIndentLevels();
  UpdateIndentGuides();

  bIsUpdatingText = false;
}

void SCodeEditableText::ToggleFoldAtLine(int32 LineNumber) {
  int32 LineIndex = LineNumber - 1;
  FCodeFoldRegion *Region = GetFoldRegionAtLine(LineIndex);
  if (Region) {
    Region->bIsFolded = !Region->bIsFolded;
    ApplyFolding();
    RebuildFoldingGutter();
  }
}

void SCodeEditableText::FoldAll() {
  for (FCodeFoldRegion &Region : FoldRegions) {
    Region.bIsFolded = true;
  }
  ApplyFolding();
  RebuildFoldingGutter();
}

void SCodeEditableText::UnfoldAll() {
  for (FCodeFoldRegion &Region : FoldRegions) {
    Region.bIsFolded = false;
  }
  ApplyFolding();
  RebuildFoldingGutter();
}

FText SCodeEditableText::GetText() const {
  return FText::FromString(OriginalText);
}

void SCodeEditableText::SetText(const FText &InText) {
  OriginalText = InText.ToString();
  DisplayedText = OriginalText;
  ParseLines();
  ParseFoldRegions();
  CalculateLineIndentLevels();
  ApplyFolding();
  RebuildFoldingGutter();
  UpdateIndentGuides();
  bIsModified = false;
}

FString SCodeEditableText::GetPlainText() const { return OriginalText; }

void SCodeEditableText::SetPlainText(const FString &InText) {
  SetText(FText::FromString(InText));
}

void SCodeEditableText::GoToLine(int32 LineNumber) {
  if (TextEditor.IsValid() && LineNumber > 0 && LineNumber <= TotalLines) {
    int32 OriginalLineIndex = LineNumber - 1;

    for (FCodeFoldRegion &Region : FoldRegions) {
      if (Region.bIsFolded && OriginalLineIndex > Region.StartLine &&
          OriginalLineIndex <= Region.EndLine) {
        Region.bIsFolded = false;
      }
    }
    ApplyFolding();
    RebuildFoldingGutter();

    int32 DisplayedLineIndex = 0;
    for (int32 i = 0; i < DisplayToOriginalLine.Num(); ++i) {
      if (DisplayToOriginalLine[i] == OriginalLineIndex) {
        DisplayedLineIndex = i;
        break;
      }
    }

    TextEditor->GoTo(FTextLocation(DisplayedLineIndex, 0));
    CurrentLine = LineNumber;
    CurrentColumn = 1;
  }
}

void SCodeEditableText::FocusEditor() {
  if (TextEditor.IsValid()) {
    FSlateApplication::Get().SetKeyboardFocus(TextEditor);
  }
}

void SCodeEditableText::HandleTextChanged(const FText &NewText) {
  if (bIsUpdatingText) {
    return;
  }

  bIsModified = true;
  OriginalText = NewText.ToString();
  DisplayedText = OriginalText;

  ParseLines();
  ParseFoldRegions();
  CalculateLineIndentLevels();
  RebuildFoldingGutter();
  UpdateIndentGuides();
  OnTextChangedCallback.ExecuteIfBound(NewText);
}

void SCodeEditableText::HandleCursorMoved(const FTextLocation &NewLocation) {
  int32 DisplayedLine = NewLocation.GetLineIndex();

  if (DisplayedLine >= 0 && DisplayedLine < DisplayToOriginalLine.Num()) {
    CurrentLine = DisplayToOriginalLine[DisplayedLine] + 1;
  } else {
    CurrentLine = DisplayedLine + 1;
  }
  CurrentColumn = NewLocation.GetOffset() + 1;

  OnCursorMovedCallback.ExecuteIfBound();
}

void SCodeEditableText::ParseLines() {
  TArray<FString> Lines;
  OriginalText.ParseIntoArray(Lines, TEXT("\n"), false);
  TotalLines = FMath::Max(1, Lines.Num());
}

#undef LOCTEXT_NAMESPACE
