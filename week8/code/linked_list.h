typedef struct Account
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    int status;
} Account;

typedef struct Node
{
    Account account;
    struct Node *next;
} Node;

typedef struct LinkedList
{
    Node *head;
} LinkedList;

LinkedList *createLinkedList()
{
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    if (list == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    list->head = NULL;
    return list;
}

void insertAccount(LinkedList *list, Account account)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newNode->account = account;
    newNode->next = list->head;
    list->head = newNode;
}

Account *findAccount(LinkedList *list, char *username)
{
    Node *current = list->head;

    while (current != NULL)
    {
        if (strcmp(current->account.username, username) == 0)
        {
            return &(current->account);
        }
        current = current->next;
    }

    return NULL;
}

void freeLinkedList(LinkedList *list)
{
    Node *current = list->head;
    while (current != NULL)
    {
        Node *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

void readAccountsFromFile(LinkedList *list, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Cannot open file\n");
        return;
    }

    Account account;
    while (fscanf(file, "%s %s %d", account.username, account.password, &account.status) != EOF)
    {
        insertAccount(list, account);
    }

    fclose(file);
}

void writeAccountsToFile(LinkedList *list, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "File opening error\n");
        return;
    }

    Node *current = list->head;
    while (current != NULL)
    {
        fprintf(file, "%s %s %d\n", current->account.username, current->account.password, current->account.status);
        current = current->next;
    }

    fclose(file);
}
