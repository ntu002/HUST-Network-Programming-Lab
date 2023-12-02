
void MakeNullTree(Tree_T *T)
{
    (*T) = NULL;
}

int EmptyTree(Tree_T T)
{
    return T == NULL;
}

node_Type *CreatT(eType x)
{
    node_Type *T = (node_Type *)malloc(sizeof(node_Type));
    if (T != NULL)
    {
        T->left = NULL;
        T->right = NULL;
        T->data = x;
    }
    return T;
}

// Tim kiem theo username
Tree_T Search(char userName[], char passWord[], Tree_T Root)
{
    if (Root == NULL)
        return NULL; // not founsd
    else if ((strcmp(Root->data.username, userName) == 0) && (strcmp(Root->data.password, passWord) == 0))
        return Root;
    else if (strcmp(Root->data.username, userName) < 0)
        // continue searching in the right sub tree
        return Search(userName, passWord, Root->right);
    else
    {
        // continue searching in the left sub tree
        return Search(userName, passWord, Root->left);
    }
}

// Tim kiem theo username
Tree_T SearchUsername(char x[], Tree_T Root)
{
    if (Root == NULL)
        return NULL; // not founsd
    else if (strcmp(Root->data.username, x) == 0)
        return Root;
    else if (strcmp(Root->data.username, x) < 0)
        // continue searching in the right sub tree
        return SearchUsername(x, Root->right);
    else
    {
        // continue searching in the left sub tree
        return SearchUsername(x, Root->left);
    }
}

// Tim kiem theo password
Tree_T SearchPassword(char x[], Tree_T Root)
{
    if (Root == NULL)
        return NULL; // not founsd
    else if (strcmp(Root->data.password, x) == 0)
        return Root;
    else
        return SearchPassword(x, Root->right);
}

void InsertNode(eType x, Tree_T *Root)
{
    if (*Root == NULL)
    {
        /* Create a new node for key x */
        *Root = (node_Type *)malloc(sizeof(node_Type));
        (*Root)->data = x;
        (*Root)->left = NULL;
        (*Root)->right = NULL;
    }
    else if (strcmp(x.username, (*Root)->data.username) < 0)
        InsertNode(x, &(*Root)->left);
    else if (strcmp(x.username, (*Root)->data.username) > 0)
        InsertNode(x, &(*Root)->right);
}

void inorderprint(Tree_T T)
{
    if (T != NULL)
    {
        inorderprint(T->left);
        printf("%-20s%-10s%-10d\n\n", T->data.username, T->data.password, T->data.status);
        inorderprint(T->right);
    }
}

eType deletemin(Tree_T *T)
{
    eType k;
    if ((*T)->left == NULL)
    {
        k = (*T)->data;
        (*T) = (*T)->right;
        return k;
    }
    else
        return deletemin(&(*T)->left);
}
void freeTree(Tree_T T)
{
    if (T != NULL)
    {
        freeTree(T->left);
        freeTree(T->right);
        free((void *)T);
    }
}
