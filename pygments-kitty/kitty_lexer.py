from pygments.lexer import RegexLexer, words, bygroups
from pygments.token import Name, Keyword, Operator, Literal, Number, Text, Comment, Punctuation


class CustomLexer(RegexLexer):
    name = 'KITTY'
    aliases = ['kitty']
    filenames = ['*.kitty']

    keywords = ['if', 'then', 'else', 'while', 'do', 'write',
                'func', 'return', 'allocate', 'length', 'of', 'end']
    declaration_keywords = ['var', 'type']
    types = ['int', 'bool', 'array', 'record']
    operators = ["=", ":", "!", "|", "+", "-", "*", "/"]
    punctuation = ["(", ")", "{", "}", "[", "]", ".", ",", ";"]

    tokens = {
        'root': [
            (r'\s+', Text),
            (r'\(\*', Comment.Multiline, 'comment'),
            (r'(func)\s*(\()', bygroups(Keyword, Text), 'par_decl_list'),
            (words(keywords, suffix=r'\b'), Keyword),
            (words(declaration_keywords, suffix=r'\b'), Keyword.Declaration),
            (words(types, suffix=r'\b'), Keyword.Type),
            (words(operators), Operator),
            (words(punctuation), Punctuation),
            (r'true|false', Keyword.Constant),
            (r'0|([1-9][0-9]*)', Number),
            (r'[a-zA-Z_][a-zA-Z0-9_]*', Name.Variable),
        ],
        'comment': [
            (r'\(\*', Comment.Multiline, '#push'),
            (r'[^*\)]', Comment.Multiline),
            (r'\*\)', Comment.Multiline, '#pop'),
            (r'[\*\)]', Comment.Multiline)
        ],
        'par_decl_list': [
            (r'([a-zA-Z_][a-zA-Z0-9_]*)\s*:\s*([^\),]),?',
             bygroups(Name.Symbol, Keyword.Type)),
            (r'\)', Text, '#pop')
        ]
    }
