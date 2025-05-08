#ifndef AST_H
#define AST_H

#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64)
#else
#ifndef stricmp
#define stricmp strcasecmp
#endif
#ifndef sprintf_s
#define sprintf_s snprintf
#endif
#ifndef Sleep
#define Sleep sleep
#endif
#endif

#define EXPR_LENGTH 256
typedef unsigned int u32, uint;
typedef unsigned short u16;
typedef unsigned char byte;
extern char *type_name[1024];

enum TYPE_ID
{
    EXPR_INTNUM = 512,
    EXPR_APPROXNUM,
    EXPR_STRING,
    EXPR_DATETIME,
    EXPR_NAME,
    EXPR_TABLE_COLUMN,
    EXPR_ADD,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV,
    EXPR_MOD,
    EXPR_AND,
    EXPR_OR,
    EXPR_XOR,
    EXPR_EQ,
    EXPR_NE,
    EXPR_LT,
    EXPR_GT,
    EXPR_LE,
    EXPR_GE,
    EXPR_NEG,
    EXPR_NOT,
    EXPR_IN_VAL_LIST,
    EXPR_NOT_IN_VAL_LIST,
    EXPR_LIKE,
    EXPR_NOT_LIKE,
    EXPR_SELECT,
    EXPR_IN_SELECT,
    EXPR_NOT_IN_SELECT,
    EXPR_VAL_LIST,
    TABLE_DEFAULT,
    TABLE_SUBQUERY,
    ORDERBY,
    GROUPBY,
    SELECT_ALL,
    SELECT_STMT,
    DELETE_STMT,
    INSERT_STMT,
    UPDATE_STMT,
    USE_STMT,
    EXPR_LAZY,
    EXPR_NULL,
    EXPR_ERROR
};

typedef struct ExprNode
{
    u16 type;
    union {
        int intval;
        float floatval;
        char *strval;
        struct SelectNode *select;
        struct ExprNode *case_head;
    };
    char *table, *alias;
    u16 op;
    byte sc;
    char text[EXPR_LENGTH];
    struct ExprNode *l, *r;
    struct ExprNode *next;
} ExprNode;

typedef struct ValueListNode
{
    struct ExprNode *head;
    struct ValueListNode *next;
} ValueListNode;

typedef struct TableNode
{
    u16 type;
    union {
        char *table;
        struct SelectNode *select; //子查询
    };
    char *alias;
    struct TableNode *next;
} TableNode;

typedef struct SetNode
{
    char *column;
    u16 op;
    struct ExprNode *expr;
    struct SetNode *next;
} SetNode;

typedef struct UseNode{
    char *db;
} UseNode;

typedef struct ColumnNode
{
    char *column;
    struct ColumnNode *next;
} ColumnNode;

typedef struct SelectNode
{
    struct ExprNode *column_head;
    struct TableNode *table_head;
    struct ExprNode *where;
    struct ExprNode *group;
    struct ExprNode *order;
    void *recs;
} SelectNode;

typedef struct DeleteNode
{
    //简易版
    char *table;
    struct ExprNode *where;
} DeleteNode;

typedef struct InsertNode
{
    struct ColumnNode *column_head;
    char *table;
    struct ValueListNode *value_list_head;
} InsertNode;

typedef struct UpdateNode
{
    char *table;
    SetNode *set_head;
    struct ExprNode *where;
} UpdateNode;

typedef struct SqlAst
{
    u16 type;
    union {
        SelectNode *select;
        DeleteNode *Delete;
        InsertNode *insert;
        UpdateNode *update;
        UseNode *use;
    };

} SqlAst;

extern SqlAst *ast_root;
extern SqlAst *parse_sql(char *sql);
extern char *my_strdup(const char *s);

#endif