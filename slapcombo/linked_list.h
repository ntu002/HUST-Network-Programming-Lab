
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

// void check(Tree_T Root)
// {
//     if (Root != NULL)
//     {
//         check(Root->left);
//         //int i;
//         if ((Root->data.inp[0] < Root->data.out[0]) && (Root->data.inp[1] < Root->data.out[1]))
//             printf("%s\n", Root->data.tt);
//         check(Root->right);

//     }
// }

// void check1(Tree_T Root)
// {
//     int i;
//     int dem;
//     float maxx = 0;
//     if (Root != NULL)
//     {
//         check1(Root->left);
//         dem =0;
//         for(i = 0; i < 10; ++i)
//             if (Root->data.out[i] - Root->data.inp[i] > maxx)
//                     maxx = Root->data.out[i] - Root->data.inp[i];
//         check1(Root->right);
//     }

//     if (Root != NULL)
//     {
//         check1(Root->left);
//         dem =0;
//         for(i = 0; i < 10; ++i)
//             if (Root->data.out[i] - Root->data.inp[i] == maxx)
//                 printf("%s  : %.3f\n", Root->data.tt, maxx);
//         check1(Root->right);
//     }
// }

void InsertNode(eType x, Tree_T *Root)
{
    if (*Root == NULL)
    {
        /* Create a new node for key x */
        *Root = (node_Type *)malloc(sizeof(node_Type));
        (*Root)->data = x;
        (*Root)->left = NULL;
        (*Root)->right = NULL;
        (*Root)->data.count = 0;
        if ((*Root)->data.status == 2)
            strcpy((*Root)->data.code, "20205135");
        // (*Root)->data.inp[0] = inp;
        // (*Root)->data.out[0] = out;
    }
    else
        // if (strcmp(x.username, (*Root)->data.username) == 0)
        // {
        //     (*Root)->data.count++;
        //     // (*Root)->data.inp[(*Root)->data.count] = inp;
        //     // (*Root)->data.out[(*Root)->data.count] = out;
        // }

        if (strcmp(x.username, (*Root)->data.username) < 0)
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
