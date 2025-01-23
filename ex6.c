#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char* str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char* myStrdup(const char* src)
{
    if (!src)
        return NULL;

    size_t len = strlen(src);
    char* dest = malloc(len + 1); 
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }

    strncpy(dest, src, len);  
    dest[len] = '\0';  

    return dest;
}


int readIntSafe(const char* prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char* endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

int openPokedexMenu(int numOfOwners)
{
    int choice;
    OwnerNode* newOwner = malloc(sizeof(OwnerNode));
    if (newOwner == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }
    memset(newOwner, 0x0, sizeof(OwnerNode));
    printf("Your name: ");
    //getting new owner's name.
    newOwner->ownerName = getDynamicInput();
    if (newOwner->ownerName == NULL)
    {
        free(newOwner);
        printf("Memory allocation failed\n");
        return 1;
    }
    //checking for duplicate names.
    if (numOfOwners != 0)
    {
        OwnerNode* currentOwner = ownerHead;
        for (int i = 0;i < numOfOwners;i++)
        {
            if (strcmp(newOwner->ownerName, currentOwner->ownerName) == 0)
            {
                printf("Owner '%s' already exists. Not creating a new Pokedex.", newOwner->ownerName);
                free(newOwner->ownerName);
                free(newOwner);
                return 1;
            }
            currentOwner = currentOwner->next;
        }
    }
    printf("Choose Starter:\n1. Bulbasaur\n2. Charmander\n3. Squirtle\n");
    choice = readIntSafe("Your choice: ");
    //putting the right starting pokemon for the new owner.
    switch (choice)
    {
    case 1:
        newOwner->pokedexRoot = createPokemonNode(&(pokedex[0]));
        break;
    case 2:
        newOwner->pokedexRoot = createPokemonNode(&(pokedex[3]));
        break;
    case 3:
        newOwner->pokedexRoot = createPokemonNode(&(pokedex[6]));
        break;
    default:
        break;
    }
    newOwner->numOfPokemons = 1;
    /*
    There arn't owners, therefore we are putting the new owner in the first place 
    and connecting him to himself from both sides.
    */
        if (numOfOwners == 0)
    {
        ownerHead = newOwner;
        newOwner->next = newOwner;
        newOwner->prev = newOwner;
    }
    else
    {
        OwnerNode* lastOwner = ownerHead->prev;
        lastOwner->next = newOwner;
        newOwner->prev = lastOwner;
        ownerHead->prev = newOwner;
        newOwner->next = ownerHead;
    }
    printf("New Pokedex created for %s with starter %s.",
    newOwner->ownerName, newOwner->pokedexRoot->data->name);
    return 0;
}

PokemonNode* createPokemonNode(const PokemonData* data)
{
    PokemonNode* newNode = malloc(sizeof(PokemonNode));
    if (newNode == NULL)
    {
        printf("Memory allcation failed");
        return NULL;
    }
    newNode->data = malloc(sizeof(PokemonData));
    if (newNode->data == NULL)
    {
        free(newNode);
        printf("Memory allocation failed\n");
        return NULL;
    }
    memset(newNode->data, 0x0, sizeof(PokemonData));
    newNode->data->attack = data->attack;
    newNode->data->CAN_EVOLVE = data->CAN_EVOLVE;
    newNode->data->hp = data->hp;
    newNode->data->id = data->id;
    strcpy(newNode->data->name,data->name);
    newNode->data->TYPE = data->TYPE;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void insertPokemonNode(PokemonNode* root, PokemonNode* newNode)
{
    int id = newNode->data->id;
    if (root == NULL) {
        root = createPokemonNode(&(pokedex[id - 1]));
    }
    if (root->data->id > id) {
        if (root->left != NULL) {
            insertPokemonNode(root->left,newNode);
            return;
        }
        root->left = createPokemonNode(&(pokedex[id - 1]));
        if (root->left == NULL)
            printf("Insertion failed.\n");
    }
    if (root->data->id < id) {
        if (root->right != NULL) {
            insertPokemonNode(root->right, newNode);
            return;
        }
        root->right = createPokemonNode(&(pokedex[id - 1]));
        if (root->right == NULL) 
            printf("Insertion failed.\n");
    }
}

int addPokemon(PokemonNode *root,int id,OwnerNode* owner)
{
    if (owner->numOfPokemons == 0)
    {
        owner->pokedexRoot = malloc(sizeof(PokemonNode));
        if (owner->pokedexRoot == NULL)
        {
            printf("Memory allocation failed\n");
            return 1;
        }
        memset(owner->pokedexRoot, 0x0, sizeof(PokemonNode));
        owner->pokedexRoot = createPokemonNode(&(pokedex[id-1]));
        printf("Pokemon %s (ID %d) added.", owner->pokedexRoot->data->name, owner->pokedexRoot->data->id);
        return 0;
    }
    if (root == NULL) {
        return -1;
    }
    if (root->data->id > id) {
        if (root->left != NULL) {
            return addPokemon(root->left, id,owner);
        }
        root->left = createPokemonNode(&(pokedex[id-1]));
        printf("Pokemon %s (ID %d) added.", root->left->data->name, root->left->data->id);
        if (root->left == NULL) {
            return -1;
        }
        else {
            return 0;
        }
    }
    if (root->data->id < id) {
        if (root->right != NULL) {
            return addPokemon(root->right, id,owner);
        }
        root->right = createPokemonNode(&(pokedex[id-1]));
        printf("Pokemon %s (ID %d) added.", root->right->data->name, root->right->data->id);
        if (root->right == NULL) {
            return -1;
        }
        else {
            return 0;
        }
    }
    printf("Pokemon with ID %d is already in the Pokedex. No changes made.", id);
    return -1;
}

PokemonNode* findMin(PokemonNode* root) {
    while (root && root->left) {
        root = root->left;
    }
    return root;
}

int removeNodeBST(PokemonNode **rootPtr, int id) {
    // If there is no pointer to head, or no head 
    if (!rootPtr || !(*rootPtr)) {
        return -1;
    } 
    PokemonNode* root = *rootPtr;
    if (id < root->data->id) {
        // go left 
        return removeNodeBST(&(root->left), id);
    }
    else
        if (id > root->data->id) {
            // go right 
            return removeNodeBST(&(root->right), id);
        } 
        else {
            PokemonNode* temp = NULL;
            if (!root->left) {
                temp = root->right;
                freePokemonNode(root);
                *rootPtr = temp;
                return 0;
            }
        else
            if (!root->right) {
                temp = root->left;
                freePokemonNode(root);
                *rootPtr = temp;
                return 0;
            }
    }
        // has two children 
        PokemonNode* temp = findMin(root->right); //find successor 
        root->data->id = temp->data->id; //replace value 
        return removeNodeBST(&(root->right), temp->data->id); //release succ
}

int freePokemon(OwnerNode* owner)
{
    if (owner->numOfPokemons == 0)
    {
        printf("No Pokemon to release.");
        return 0;
    }
    PokemonNode* root = owner->pokedexRoot;
    if (owner->numOfPokemons == 0)
        return 0;
    int id = readIntSafe("Enter Pokemon ID to release: ");
    PokemonNode* currentPokemon = searchPokemon(root, id, owner->numOfPokemons);
    if (currentPokemon == NULL)
    {
        printf("No Pokemon with ID %d found.", id);
        return 0;
    }
    printf("Removing Pokemon %s (ID %d).",currentPokemon->data->name,id);
    removeNodeBST(&root, id);
    return 1;
}



// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char* getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

PokemonNode* searchPokemon(PokemonNode* root, int id,int numOfPokemons)
{
    if (numOfPokemons == 0)
        return NULL;
    if (root == NULL)
        return NULL;
    if (root->data->id > id)
    {
        if (root->left != NULL)
            return searchPokemon(root->left, id, numOfPokemons);
        return NULL;
    }
    if (root->data->id < id)
    {
        if (root->right != NULL)
            return searchPokemon(root->right, id, numOfPokemons);
        return NULL;
    }
    return root;
}

void pokemonFight(OwnerNode* owner)
{
    int numOfPokemons = owner->numOfPokemons;
    if (numOfPokemons == 0)
    {
        printf("Pokedex is empty.");
        return;
    }
    int firstId = readIntSafe("Enter ID of the first Pokemon: ");
    int secondId = readIntSafe("Enter ID of the second Pokemon: ");
    PokemonNode* firstPokemon;
    PokemonNode* secondPokemon;
    firstPokemon = searchPokemon(owner->pokedexRoot,firstId,numOfPokemons);
    secondPokemon = searchPokemon(owner->pokedexRoot, secondId, numOfPokemons);
    if (firstPokemon == NULL || secondPokemon == NULL)
    {
        printf("One or both Pokemon IDs not found.");
        return;
    }
    double firstPokemonScore = (firstPokemon->data->attack) * 1.5 + (firstPokemon->data->hp) * 1.2;
    double secondPokemonScore = (secondPokemon->data->attack) * 1.5 + (secondPokemon->data->hp) * 1.2;
    printf("Pokemon 1: %s (Score = %.2f)\n", firstPokemon->data->name, firstPokemonScore);
    printf("Pokemon 2: %s (Score = %.2f)\n", secondPokemon->data->name, secondPokemonScore);
    if (firstPokemonScore > secondPokemonScore)
        printf("%s wins!\n", firstPokemon->data->name);
    if (firstPokemonScore < secondPokemonScore)
        printf("%s wins!\n", secondPokemon->data->name);
    if (firstPokemonScore == secondPokemonScore)
        printf("It's a tie!\n");
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char* getDynamicInput()
{
    char* input = NULL;
    size_t size = 0, capacity = 1;
    input = (char*)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char* temp = (char*)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode* node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
        node->data->id,
        node->data->name,
        getTypeName(node->data->TYPE),
        node->data->hp,
        node->data->attack,
        (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------

void preOrderTraversal(PokemonNode* root)
{
    if (root == NULL) {
        return;
    }
    printPokemonNode(root);
    preOrderTraversal(root->left);
    preOrderTraversal(root->right);
}

void inOrderTraversal(PokemonNode* root)
{
    if (root == NULL) {
        return;
    }
    inOrderTraversal(root->left);
    printPokemonNode(root);
    inOrderTraversal(root->right);
}

void postOrderTraversal(PokemonNode* root)
{
    if (root == NULL) {
        return;
    }
    postOrderTraversal(root->left);
    postOrderTraversal(root->right);
    printPokemonNode(root);
}

void preOrderGeneric(PokemonNode* root, VisitNodeFunc visit)
{
    visit(root);
}

void inOrderGeneric(PokemonNode* root, VisitNodeFunc visit)
{
    visit(root);
}

void postOrderGeneric(PokemonNode* root, VisitNodeFunc visit)
{
    visit(root);
}

void BFSGeneric(PokemonNode* root, VisitNodeFunc visit)
{
    visit(root);
}
void displayAlphabetical(OwnerNode* owner)
{
    PokemonNode** queue = malloc((owner->numOfPokemons) * sizeof(PokemonNode*));
    queue[0] = owner->pokedexRoot;
    int insertCounter = 0;
    int counter = 1;
    while (insertCounter != counter)
    {
        if (queue[insertCounter]->left != NULL)
        {
            queue[counter] = queue[insertCounter]->left;
            counter++;
        }
        if (queue[insertCounter]->right != NULL)
        {
            queue[counter] = queue[insertCounter]->right;
            counter++;
        }
        insertCounter++;
    }
    for (int i = 0; i < owner->numOfPokemons - 1; i++)
    {
        for (int j = 0; j < owner->numOfPokemons - 1 - i; j++)
        {
            if (shouldSwap(queue[j]->data->name, queue[j + 1]->data->name))
                swapPokemonData(queue[j], queue[j + 1]);
        }
    }
    for (int i = 0; i < owner->numOfPokemons;i++)
        printPokemonNode(queue[i]);
    free(queue);
}
void displayBFS(PokemonNode* root)
{
    PokemonNode** queue = malloc(sizeof(PokemonNode*));
    queue[0] = root;
    int printCounter = 0;
    int counter = 1;
    while (printCounter != counter)
    {
        if (queue[printCounter]->left != NULL)
        {
            queue = realloc(queue, (counter + 1) * sizeof(PokemonNode*));
            queue[counter] = queue[printCounter]->left;
            counter++;
        }
        if (queue[printCounter]->right != NULL)
        {
            queue = realloc(queue, (counter + 1) * sizeof(PokemonNode*));
            queue[counter] = queue[printCounter]->right;
            counter++;
        }
        printPokemonNode(queue[printCounter]);
        printCounter++;
    }
    free(queue);
}
void displayMenu(OwnerNode* owner)  
{
    if (owner->numOfPokemons == 0)
    {
        printf("Pokedex is empty.\n");
        return;
    }
    VisitNodeFunc visit;
        if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }
        

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        visit = displayBFS;
        BFSGeneric(owner->pokedexRoot,visit);
        break;
    case 2:
        visit = preOrderTraversal;
        preOrderGeneric(owner->pokedexRoot, visit);
        //preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        visit = inOrderTraversal;
        inOrderGeneric(owner->pokedexRoot, visit);
        //inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        visit = postOrderTraversal;
        postOrderGeneric(owner->pokedexRoot, visit);
        //postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        displayAlphabetical(owner);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

void evolvePokemon(OwnerNode* owner)
{
    PokemonNode* root = owner->pokedexRoot;
    int numOfPokemons = owner->numOfPokemons;
    if (numOfPokemons == 0)
    {
        printf("Cannot evolve. Pokedex empty.");
        return;
    }
    int oldId = readIntSafe("Enter ID of Pokemon to evolve: \n");
    int newId = oldId + 1;
    PokemonNode* oldPokemon = searchPokemon(owner->pokedexRoot,oldId, numOfPokemons);
    PokemonNode* newPokemon = searchPokemon(owner->pokedexRoot,newId, numOfPokemons);
    if (newPokemon != NULL)
    {
        printf("Evolution ID %d (%s)already in the Pokedex.Releasing %s(ID %d).\n", newId, newPokemon->data->name
            , oldPokemon->data->name, oldId);
        printf("Removing Pokemon %s (ID %d).", oldPokemon->data->name, oldId);
        removeNodeBST(&root, oldId);
        return;
    }
    if (oldPokemon == NULL)
    {
        printf("No Pokemon with ID %d found.\n", oldId);
        return;
    }
    if (oldPokemon->data->CAN_EVOLVE == 0)
    {
        printf("%s (ID %d) cannot evolve.",oldPokemon->data->name,oldPokemon->data->id);
        return;
    }
    root->data->attack = pokedex[newId].attack;
    root->data->CAN_EVOLVE = pokedex[newId].CAN_EVOLVE;
    root->data->hp = pokedex[newId].hp;
    root->data->id = pokedex[newId].id;
    strcpy(root->data->name, pokedex[newId].name);
    root->data->TYPE = pokedex[newId].TYPE;
}

void freePokemonNode(PokemonNode* node)
{
    free(node->data);
    free(node);
}

void freePokemonTree(PokemonNode* root)
{
    if (root == NULL)
        return;
    freePokemonTree(root->left);
    freePokemonTree(root->right);
    freePokemonNode(root);
}

void freeOwnerNode(OwnerNode* owner)
{
    freePokemonTree(owner->pokedexRoot);
    free(owner->ownerName);
    free(owner);
}

int deletePokedex(int numOfOwners)
{
    if (numOfOwners == 0)
    {
        printf("No existing Pokedexes to delete.\n");
        return -1;
    }
    OwnerNode* currentOwner = ownerHead;
    OwnerNode* nextOwner = ownerHead;
    OwnerNode* prevOwner = ownerHead;
    int chosenOwner;
    printf("\n=== Delete a Pokedex ===\n");
    for (int i = 1; i <= numOfOwners; i++)
    {
        printf("%d. %s\n", i, currentOwner->ownerName);
        currentOwner = currentOwner->next;;
    }
    currentOwner = ownerHead;
    chosenOwner = readIntSafe("Choose a Pokedex to delete by number: ");
    for (int j = 1; j < chosenOwner;j++)
        currentOwner = currentOwner->next;
    printf("Deleting %s's entire Pokedex...\n", currentOwner->ownerName);
    if (numOfOwners > 1)
    {
        nextOwner = currentOwner->next;
        prevOwner = currentOwner->prev;
        prevOwner->next = nextOwner;
        nextOwner->prev = prevOwner;
    }
    else
    {
        ownerHead = NULL;
    }
    if (currentOwner == ownerHead)
        ownerHead = ownerHead->next;
    freeOwnerNode(currentOwner);
    printf("Pokedex deleted.\n");
    return 0;
}

void printOwnersCircular()
{
    char direction;
    printf("Enter direction (F or B): ");
    char buffer[INT_BUFFER];
    OwnerNode* currentOwner = ownerHead;
    int printAmount;
    if (!fgets(buffer, sizeof(buffer), stdin))
    {
        printf("Invalid input.\n");
        clearerr(stdin);
    }
    trimWhitespace(buffer);
    direction = buffer[0];
    if ((direction != 'f') && (direction != 'F') && (direction != 'b') && (direction != 'B'))
    {
        printf("Invalid direction, must be F or B.");
        return;
    }
    printAmount = readIntSafe("How many prints? ");
    if ((direction == 'f') || (direction == 'F'))
    {
        for (int i = 1; i < printAmount+1; i++)
        {
            printf("[%d] %s\n", i, currentOwner->ownerName);
            currentOwner = currentOwner->next;
        }
        return;
    }
    if ((direction == 'b') || (direction == 'B'))
    {
        for (int i = 1; i < printAmount + 1; i++)
        {
            printf("[%d] %s\n", i, currentOwner->ownerName);
            currentOwner = currentOwner->prev;
        }
        return;
    }
}

void swapOwnerData(OwnerNode* a, OwnerNode* b) 
{
    OwnerNode* temp = malloc(sizeof(OwnerNode));
    temp->numOfPokemons = a->numOfPokemons; 
    temp->ownerName = a->ownerName;
    temp->pokedexRoot = a->pokedexRoot;
    a->numOfPokemons = b->numOfPokemons;
    a->ownerName = b->ownerName;
    a->pokedexRoot = b->pokedexRoot;
    b->numOfPokemons = temp->numOfPokemons;
    b->ownerName = temp->ownerName;
    b->pokedexRoot = temp->pokedexRoot;
    free(temp);
}

void swapPokemonData(PokemonNode* a, PokemonNode* b)
{
    PokemonNode* temp = malloc(sizeof(PokemonNode));
    temp->data = a->data;
    a->data = b->data;
    b->data = temp->data;
    free(temp);
}

int shouldSwap(char name1[], char name2[])
{
    return(strcmp(name1, name2) > 0);
}

void sortOwners(int numOfOwners)
    {
    if (numOfOwners == 0)
    {
        printf("Pokedex is empty.\n");
        return;
    }
    if (numOfOwners == 1)
    {
        printf("Owners sorted by name.\n");
        return;
    }
    OwnerNode* currentOwner = ownerHead;
    OwnerNode* nextOwner = currentOwner->next;
    for (int i = 0; i < numOfOwners - 1; i++)
    {
        currentOwner = ownerHead;
        nextOwner = currentOwner->next;
        for (int j = 0; j < numOfOwners - 1 - i; j++)
        {
            if (strcmp(nextOwner->ownerName, currentOwner->ownerName) < 0)
            {
                swapOwnerData(nextOwner, currentOwner);
                currentOwner = nextOwner;
                nextOwner = nextOwner->next;
            }
            else
            {
                currentOwner = nextOwner;
                nextOwner = currentOwner->next;
            }
        }   
    }
    printf("Owners sorted by name.\n");
    return;
}

void removeOwnerFromCircularList(OwnerNode* target)
{
    OwnerNode* nextOwner = target->next;
    OwnerNode* prevOwner = target->prev;
    if (prevOwner != NULL)
        prevOwner->next = nextOwner;
    if (nextOwner != NULL)
        nextOwner->prev = prevOwner;
    if (target == ownerHead)
        ownerHead = ownerHead->next;
    freeOwnerNode(target);
}

OwnerNode* findOwnerByName(const char* name,int numOfOwners)
{
    OwnerNode* currentOwner = ownerHead;
    for (int i = 0;i < numOfOwners;i++)
    {
        if (strcmp(currentOwner->ownerName, name) == 0)
            return currentOwner;
        currentOwner = currentOwner->next;
    }
    return NULL;
}

void freeAllOwners() 
{
    OwnerNode* currentOwner = ownerHead;
    currentOwner->prev->next = NULL;
    while (currentOwner != NULL)
    {
        OwnerNode* tempOwner = currentOwner->next;
        freeOwnerNode(currentOwner);
        currentOwner = tempOwner;
    }
}

int mergePokedexMenu(int numOfOwners)
{
    if (numOfOwners < 2)
    {
        printf("Not enough owners to merge.");
        return -1;
    }
    printf("\n=== Merge Pokedexes ===\n");
    printf("Enter name of first owner: ");
    char *ownerName = getDynamicInput();
    if (ownerName == NULL)
    {
        printf("Memory allocation failed in mergePokedexMenu.\n");
        return -1;
    }
    OwnerNode* firstOwner = findOwnerByName(ownerName, numOfOwners);
    free(ownerName);
    printf("Enter name of second owner: ");
    ownerName = getDynamicInput();
    if (ownerName == NULL)
    {
        printf("Memory allocation failed in mergePokedexMenu.\n");
        return -1;
    }
    OwnerNode* secondOwner = findOwnerByName(ownerName, numOfOwners);
    free(ownerName);
    printf("Merging %s and %s...\n",firstOwner->ownerName,secondOwner->ownerName);
    printf("Merge completed.\n");
    printf("Owner '%s' has been removed after merging.\n",secondOwner->ownerName);
    PokemonNode** queue = malloc((secondOwner->numOfPokemons)*sizeof(PokemonNode*));
    queue[0] = secondOwner->pokedexRoot;
    int insertCounter = 0;
    int counter = 1;
    while (insertCounter != counter)
    {
        if (queue[insertCounter]->left != NULL)
        {
            queue[counter] = queue[insertCounter]->left;
            counter++;
        }
        if (queue[insertCounter]->right != NULL)
        {    
            queue[counter] = queue[insertCounter]->right;
            counter++;
        }
        insertPokemonNode(firstOwner->pokedexRoot, queue[insertCounter]);
        insertCounter++;
    }
    free(queue);
    //freeOwnerNode(firstOwner);
    removeOwnerFromCircularList(secondOwner);
    
    return 0;
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu(int numOfOwners)
{
    if (numOfOwners == 0)
    {
        printf("No existing Pokedexes.\n");
        return;
    }
    // list owners
    printf("\nExisting Pokedexes:\n");
    int i = 1;
    OwnerNode* currentOwner = ownerHead; //current owner for printing.
    OwnerNode* cur = ownerHead;// current owner for inner functions.
    for (int j = 0; j < numOfOwners;j++)
    {
        printf("%d. %s\n", i, currentOwner->ownerName);
        currentOwner = currentOwner->next;
        i++;
    }
    int chosenOwner;
    chosenOwner = readIntSafe("Choose a Pokedex by number: ");
    for (int j = 0; j < chosenOwner-1;j++)
        cur = cur->next;
    printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        int id;
        subChoice = readIntSafe("Your choice: ");
        switch (subChoice)
        {
        case 1:
            id = readIntSafe("Enter ID to add: ");
            if (id > MAX_ID && id < MIN_ID)
            {
                printf("Invalid ID.\n");
                break;
            }
            if(addPokemon(cur->pokedexRoot,id,cur) == 0)
                cur->numOfPokemons = cur->numOfPokemons + 1;
            break;
        case 2:
            displayMenu(cur);
            break;
        case 3:
            if (freePokemon(cur))
                cur->numOfPokemons = cur->numOfPokemons - 1;
            break;
        case 4:
            pokemonFight(cur);
            break;
        case 5:
            evolvePokemon(cur);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu(int numOfOwners)
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");     
        switch (choice)
        {
        case 1:
            if (openPokedexMenu(numOfOwners) == 0)
                numOfOwners++;
            break;
        case 2:
            enterExistingPokedexMenu(numOfOwners);
            break;
        case 3:
            if (deletePokedex(numOfOwners) == 0)
                numOfOwners--;
            break;
        case 4:
            if (mergePokedexMenu(numOfOwners) == 0)
                numOfOwners--;
            break;
        case 5:
            sortOwners(numOfOwners);
            break;
        case 6:
            if (numOfOwners == 0)
            {
                printf("Pokedex is empty.\n");
                break;
            }
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu(0);
    freeAllOwners();
    return 0;
}
