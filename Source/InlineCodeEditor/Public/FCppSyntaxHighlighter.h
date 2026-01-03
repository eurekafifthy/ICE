// Copyright Yureka. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Text/SyntaxHighlighterTextLayoutMarshaller.h"
#include "Framework/Text/SyntaxTokenizer.h"

/**
 * Token types for C++ syntax highlighting (Monaco-style)
 * Order matters for casting to ETokenType
 */
enum class ECppTokenType : uint8 {
  Normal,        // Default text
  Comment,       // // and /* */
  PreProcessor,  // #include, #define, etc.
  Keyword,       // if, for, class, etc.
  ControlFlow,   // return, break, continue, throw
  Type,          // int, float, FString, etc.
  UnrealMacro,   // UCLASS, UPROPERTY, etc.
  String,        // "..." and '...'
  Number,        // 123, 0xFF, 3.14f
  Operator,      // + - * / etc.
  Punctuation,   // { } [ ] ( ) ; ,
  FunctionCall,  // Function names followed by (
  ClassName,     // Class/struct names after class keyword
  Namespace,     // namespace names, ::
  MemberAccess,  // . and ->
  TemplateParam, // <T> template parameters
  IncludePath,   // <file.h> or "file.h" in #include
  Escape,        // \n \t etc inside strings
};

/**
 * Custom tokenizer for C++ code (Monaco-style highlighting)
 * Provides advanced lexical analysis:
 * - Function calls detection
 * - Namespace detection
 * - Template parameter detection
 * - Multi-line comments
 * - Advanced preprocessor handling
 */
class INLINECODEEDITOR_API FCppSyntaxTokenizer : public ISyntaxTokenizer {
public:
  static TSharedRef<FCppSyntaxTokenizer> Create();
  virtual ~FCppSyntaxTokenizer() = default;

  // ISyntaxTokenizer interface
  virtual void Process(TArray<FTokenizedLine> &OutTokenizedLines,
                       const FString &Input) override;

private:
  FCppSyntaxTokenizer();

  /** Check token type helper */
  ECppTokenType GetTokenType(const FString &Token, bool bFollowedByParen,
                             bool bAfterClassKeyword, bool bAfterNamespace,
                             bool bAfterScopeResolution) const;

  /** Check if character is operator-like */
  bool IsOperatorChar(TCHAR C) const;

  TSet<FString> Keywords;
  TSet<FString> ControlFlowKeywords;
  TSet<FString> BuiltInTypes;
  TSet<FString> UnrealTypes;
  TSet<FString> UnrealMacros;
  TSet<FString> PreProcessorDirectives;

  /** State tracking for multi-line features */
  bool bInBlockComment = false;
};

/**
 * Syntax highlighter marshaller for C++ (Monaco-style)
 */
class INLINECODEEDITOR_API FCppSyntaxHighlighter
    : public FSyntaxHighlighterTextLayoutMarshaller {
public:
  static TSharedRef<FCppSyntaxHighlighter> Create();
  virtual ~FCppSyntaxHighlighter() = default;

protected:
  FCppSyntaxHighlighter(TSharedPtr<ISyntaxTokenizer> InTokenizer);

  // FSyntaxHighlighterTextLayoutMarshaller interface
  virtual void
  ParseTokens(const FString &SourceString, FTextLayout &TargetTextLayout,
              TArray<ISyntaxTokenizer::FTokenizedLine> TokenizedLines) override;

private:
  // Monaco Dark+ Theme colors
  FTextBlockStyle NormalTextStyle;   // #D4D4D4 - Light gray
  FTextBlockStyle CommentTextStyle;  // #6A9955 - Green
  FTextBlockStyle StringTextStyle;   // #CE9178 - Orange/salmon
  FTextBlockStyle KeywordTextStyle;  // #569CD6 - Blue
  FTextBlockStyle ControlFlowStyle;  // #C586C0 - Purple/magenta
  FTextBlockStyle TypeTextStyle;     // #4EC9B0 - Teal/cyan
  FTextBlockStyle MacroTextStyle;    // #DCDCAA - Yellow
  FTextBlockStyle NumberTextStyle;   // #B5CEA8 - Light green
  FTextBlockStyle OperatorTextStyle; // #D4D4D4 - Light gray
  FTextBlockStyle PunctuationStyle;  // #808080 - Dim gray
  FTextBlockStyle FunctionCallStyle; // #DCDCAA - Yellow (like macros)
  FTextBlockStyle ClassNameStyle;    // #4EC9B0 - Teal (like types)
  FTextBlockStyle NamespaceStyle;    // #4EC9B0 - Teal
  FTextBlockStyle PreProcessorStyle; // #C586C0 - Purple
  FTextBlockStyle IncludePathStyle;  // #CE9178 - Orange (like strings)
};
