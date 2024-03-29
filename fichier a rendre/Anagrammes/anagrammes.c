#include "anagrammes.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction permettant d'augmenter la capacité d'un tableau de mot
static void word_array_grow(struct word_array *self) {
    size_t capacity = self->capacity * 2;
    char **new_array = calloc(capacity, sizeof(char *));
    memcpy(new_array, self->data, self->size * sizeof(char *));
    free(self->data);
    self->data = new_array;
    self->capacity = capacity;
}

/******************* Part 1 *********************/

bool string_are_anagrams(const char *str1, const char *str2) {
    // Si deux mots n'ont pas la même taille ils ne peuvent pas être des
    // anagrammes, donc on arrête le traitement
    size_t length = strlen(str1);
    if (length != strlen(str2)) {
        return false;
    }
    // Constante pour la taille d'un tableau contenant les 26 lettres de
    // l'alphabet + une case pour les jokers
    const size_t LETTER_NUMBER = 27;
    // On crée un tableau pour compter les occurences des 26 lettres de
    // l'alphabet et des jokers
    int *tab_letters_count = calloc(LETTER_NUMBER, sizeof(int));
    // On initialise toutes les valeurs du tableau à 0
    for (size_t i = 0; i < LETTER_NUMBER; i++) {
        tab_letters_count[i] = 0;
    }
    // On parcourt la première chaine et on incrémente le nombre de chaque
    // lettre à chaque fois qu'elle apparait dans le mot. En parallèle on
    // parcourt la deuxième chaine et on décrémente à chaque occurence des
    // lettres
    size_t letter_num_1 = 0;
    size_t letter_num_2 = 0;
    for (size_t i = 0; i < length; ++i) {
        // S'il y a un joker on l'ajoute dans la 27ème case du tableau
        if (str1[i] == '*' && tab_letters_count[LETTER_NUMBER - 1] < 4) {
            letter_num_1 = LETTER_NUMBER - 1;
        } else {
            letter_num_1 = str1[i] - 'a';
        }
        letter_num_2 = str2[i] - 'a';
        // On s'assure qu'il n'y ai pas d'accès hors du tableau possible
        if (letter_num_1 >= LETTER_NUMBER || letter_num_2 >= LETTER_NUMBER) {
            free(tab_letters_count);
            return false;
        }
        assert(letter_num_1 < LETTER_NUMBER);
        assert(letter_num_2 < LETTER_NUMBER);
        tab_letters_count[letter_num_1]++;
        tab_letters_count[letter_num_2]--;
    }
    // Si toutes les cases (donc le compte des lettres) sont égales à 0, alors
    // les mots sont des anagrammes
    size_t i = 0;
    while (i < LETTER_NUMBER) {
        // Si le compteur de la lettre est égal à 0, on passe à la lettre
        // suivante
        if (tab_letters_count[i] == 0) {
            i++;
            continue;
        }
        if (tab_letters_count[i] > 0) {
            // Si une lettre de la premiere chaine n'est pas dans la deuxième,
            // alors les mots ne sont pas des anagrammes
            free(tab_letters_count);
            return false;
        }

        // Si une lettre de la 2eme chaine est n'est pas dans la première,
        // on l'incrémente mais on décrémente le compteur de jokers pour
        // conpenser. Si celui-ci est négatif c'est que les mots ne sont pas
        // des anagrammes
        tab_letters_count[LETTER_NUMBER - 1]--;
        tab_letters_count[i]++;
        if (tab_letters_count[LETTER_NUMBER - 1] < 0) {
            free(tab_letters_count);
            return false;
        }
        // On ne passe à la lettre suivante que si il ne reste plus d'occurences
        // de celle en cours à traiter
        if (tab_letters_count[i] == 0) {
            i++;
        }
    }
    // Si les conditions précédentes sont passées c'est que les mots sont des
    // anagrammes
    free(tab_letters_count);
    return true;
}

char *string_duplicate(const char *str) {
    char *st = calloc(strlen(str) + 1, sizeof(char));
    return strcpy(st, str);
}

void string_sort_letters(char *str) {
    // Tri par insertion
    size_t size = strlen(str);
    for (size_t i = 1; i < size; ++i) {
        char x = str[i];
        size_t j = i;
        while (j > 0 && str[j - 1] > x) {
            str[j] = str[j - 1];
            j--;
        }
        str[j] = x;
    }
}

void clean_newline(char *buf, size_t size) {
    // On remplace le caractère de fin de ligne par le caractère nul
    for (size_t i = 0; i < size; i++) {
        if (buf[i] == '\n' || buf[i] == '\r') {
            buf[i] = '\0';
            return;
        }
    }
}

/******************* Part 2 *********************/
// Fonction pour copier un tableau de mot
static void word_array_copy(const struct word_array *source,
                            struct word_array *dest) {
    for (size_t i = 0; i < source->size; i++) {
        word_array_add(dest, *(source->data + i));
    }
}

void word_array_create(struct word_array *self) {
    self->size = 0;
    self->capacity = 10;
    self->data = calloc(self->capacity, sizeof(char **));
    *self->data = NULL;
}

// Détruit un tableau de mot
void word_array_destroy(struct word_array *self) {
    for (size_t i = 0; i < self->capacity; i++) {
        free(*(self->data + i));
    }
    self->capacity = 0;
    self->size = 0;
    free(self->data);
}

// Ajoute une string à la fin du tableau
void word_array_add(struct word_array *self, const char *word) {
    char *p = string_duplicate(word);
    if (self->size >= self->capacity) {
        word_array_grow(self);
    }
    self->data[self->size] = p;
    self->size++;
}

void word_array_search_anagrams(const struct word_array *self, const char *word,
                                struct word_array *result) {
    // On crée un tableau de mots temporaire
    struct word_array *tmp_word_array = malloc(sizeof(struct word_array));
    word_array_create(tmp_word_array);

    // Copie de self dans le temporaire afin de pouvoir traiter ses mots sans
    // changer l'original
    word_array_copy(self, tmp_word_array);
    // Recherche des anagrammes en O(n²)
    for (size_t i = 0; i < tmp_word_array->size; i++) {
        if (string_are_anagrams(word, *(tmp_word_array->data + i))) {
            word_array_add(result, *(tmp_word_array->data + i));
        }
    }
    // Libération de la mémoire
    word_array_destroy(tmp_word_array);
    free(tmp_word_array);
}

// Fonctions nécéssaires au quick sort
// Echange de données
static void array_swap(char **data, size_t i, size_t j) {
    char *tmp = data[i];
    data[i] = data[j];
    data[j] = tmp;
}

// partitionnemenent d'un tableau
static ptrdiff_t array_partition(char **data, ptrdiff_t i, ptrdiff_t j) {
    ptrdiff_t pivot_index = i;
    const char *pivot = data[pivot_index];
    array_swap(data, pivot_index, j);
    ptrdiff_t l = i;
    for (ptrdiff_t k = i; k < j; ++k) {
        if (strcmp(data[k], pivot) < 0) {
            array_swap(data, k, l);
            l++;
        }
    }
    array_swap(data, l, j);
    return l;
}

// Tri récursif des deux moitiés du tableau
static void array_quick_sort_partial(char **data, ptrdiff_t i, ptrdiff_t j) {
    if (i < j) {
        ptrdiff_t p = array_partition(data, i, j);
        array_quick_sort_partial(data, i, p - 1);
        array_quick_sort_partial(data, p + 1, j);
    }
}
// Tri optimal pour l'array sort
static void array_quick_sort(char **data, size_t n) {
    array_quick_sort_partial(data, 0, n - 1);
}

void word_array_sort(struct word_array *self) {
    array_quick_sort(self->data, self->size);
}

void word_array_print(const struct word_array *self) {
    for (size_t i = 0; i < self->size; i++) {
        printf("%s\n", self->data[i]);
    }
}

void word_array_read_file(struct word_array *self, const char *filename) {
    char word[WORD_LETTERS_MAX];

    FILE *fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Error when opening file '%s'\n", filename);
        return;
    }

    while (!feof(fp)) {
        fgets(word, WORD_LETTERS_MAX, fp);
        if (!feof(fp)) {
            clean_newline(word, WORD_LETTERS_MAX);
            word_array_add(self, word);
        }
    }

    fclose(fp);
}

/******************* Part 3 *********************/

struct word_dict_bucket *word_dict_bucket_add(struct word_dict_bucket *bucket,
                                              const char *word) {
    struct word_dict_bucket *new_bucket =
        malloc(sizeof(struct word_dict_bucket));
    new_bucket->word = word;
    new_bucket->next = bucket;
    return new_bucket;
}

void word_dict_bucket_destroy(struct word_dict_bucket *bucket) {
    struct word_dict_bucket *curr = bucket;
    while (bucket != NULL) {
        bucket = bucket->next;
        free(curr);
        curr = bucket;
    }
}

void word_dict_create(struct word_dict *self) {
    self->count = 0;
    self->size = 10;
    self->buckets = calloc(self->size, sizeof(struct word_dict_bucket *));
    for (size_t i = 0; i < self->size; i++) {
        self->buckets[i] = NULL;
    }
}

void word_dict_destroy(struct word_dict *self) {
    // On crée un pointeur curr qui pointe sur les cases du tableau(qui eux
    // pointent sur une liste)
    // Pour toutes les cases du tableau
    for (size_t i = 0; i < self->size; i++) {
        if (self->buckets != NULL) {
            // si le pointeur n'est pas NULL
            if (self->buckets[i] != NULL) {
                word_dict_bucket_destroy(self->buckets[i]);
            }
        }
    }
    free(self->buckets);
    // On réinitialise le nombre d'éléments et la taille du tableau à 0
    self->count = 0;
    self->size = 0;
}
size_t fnv_hash(const char *key) {
    // On duplique la clé afin de pouvoir la triée
    char *key_cpy = string_duplicate(key);
    string_sort_letters(key_cpy);
    // On défini les constantes nécéssaires au hachâge
    const size_t FNV_offset_basis = 14695981039346656037ul;
    const size_t FNV_prime = 1099511628211ul;
    // Algorithme FNV-1a adapté de wikipédia
    size_t hash = FNV_offset_basis;
    size_t i = 0;
    while (key_cpy[i] != '\0') {
        hash = hash ^ key_cpy[i];
        hash = hash * FNV_prime;
        i++;
    }
    // On libère l'espace pris par la copie de la clé
    free(key_cpy);
    return hash;
}

void word_dict_rehash(struct word_dict *self) {
    // On crée un tableau de taille doublée
    struct word_dict *self2 = malloc(sizeof(struct word_dict));
    self2->size = self->size * 2;
    self2->count = 0;
    self2->buckets = calloc(self2->size, sizeof(struct word_dict_bucket *));

    // On ajoute chaque mot du tableau initial dans le tableau de taille
    // doublée
    struct word_dict_bucket *pointer = NULL;
    for (size_t i = 0; i < self->size; ++i) {
        pointer = self->buckets[i];
        while (pointer != NULL) {
            word_dict_add(self2, pointer->word);
            pointer = pointer->next;
        }
    }

    // On free le tableau initial et on le remplace par le nouveau tableau
    word_dict_destroy(self);
    self->buckets = self2->buckets;
    self->count = self2->count;
    self->size = self2->size;

    // On libère la table de transition
    self2->buckets = NULL;
    free(self2);
}

void word_dict_add(struct word_dict *self, const char *word) {
    // On teste si le facteur de compression est supérieur à 0,5
    if (self->count / self->size >= 0.5) {
        word_dict_rehash(self);
    }
    // Ensuite on ajoute le mot dans le bucket du dictionnaire à l'indice du
    // hash modulo la taille du dictionnaire
    const size_t hash = fnv_hash(word);
    const size_t index = hash % self->size;
    self->buckets[index] = word_dict_bucket_add(self->buckets[index], word);
    self->count++;
}

void word_dict_fill_with_array(struct word_dict *self,
                               const struct word_array *array) {
    // On ajoute chaque mot du tableau de mot contenant le dictionnaire dans la
    // table de hachage
    for (size_t i = 0; i < array->size; i++) {
        word_dict_add(self, array->data[i]);
    }
}

void word_dict_search_anagrams(const struct word_dict *self, const char *word,
                               struct word_array *result) {

    // On hache le mot entré par l'utilisateur puis on va chercher dans le
    // bucket dont la clé correspond à ce hash
    size_t hash = fnv_hash(word);
    const size_t index = hash % self->size;
    struct word_dict_bucket *anagrammes = self->buckets[index];
    while (anagrammes != NULL) {
        // On vérifie que les strings qui sont trouvées sont bien des anagrammes
        // afin de ne pas prendre les collisions en compte
        if (string_are_anagrams(word, anagrammes->word)) {
            word_array_add(result, anagrammes->word);
        }
        anagrammes = anagrammes->next;
    }
}

/******************* Part 4 *********************/
// Implémentation de ces fonctions finalement utilisée seulement pour les tables
// de hachage car il a été jugé plus intuitif de les prendre directement en
// compte dans la fonction string_are_anagrams pour le tableau de mots

// Initialisation d'un joker
void wildcard_create(struct wildcard *self) { self->count = 0; }

// Recherche des jokers dans un mot et renseignement de l'indice de leur
// emplacement
void wildcard_search(struct wildcard *self, const char *word) {
    size_t i = 0;
    while (word[i] != '\0') {
        if (self->count >= 4) {
            return;
        }
        if (word[i] == '*') {
            self->index[self->count] = i;
            self->count++;
        }
        i++;
    }
}

void word_array_search_anagrams_wildcard(const struct word_array *self,
                                         const char *word,
                                         struct word_array *result) {
    // On appelle la fonction de base puisque les wildcards sont déjà gérer
    // dedans au lieu de dupliquer le code
    word_array_search_anagrams(self, word, result);
}

void word_dict_search_anagrams_wildcard(const struct word_dict *self,
                                        const char *word,
                                        struct word_array *result) {
    struct wildcard *joker = malloc(sizeof(struct wildcard));
    char *new_word = string_duplicate(word);
    wildcard_create(joker);
    // On compte le nombre de joker dans le mot entré par l'utilisateur
    wildcard_search(joker, new_word);

    // On fait un switch sur le nombre de joker pour pouvoir itérer le nombre de
    // fois approprié
    switch (joker->count) {
        // Il n'y a pas de joker, on appelle la fonction de base
    case 0:
        word_dict_search_anagrams(self, word, result);
        break;

    // Il y a 1 joker, on teste toutes les lettres de l'alphabet à la place
    // qu'il occupe
    case 1:
        for (size_t i = 0; i <= 26; ++i) {
            new_word[joker->index[0]] = i + 'a';
            word_dict_search_anagrams(self, new_word, result);
        }
        break;

    // Même chose que pour 1 joker mais en testant toutes les combinaisons
    // possibles en remplaçant deux lettre
    case 2:
        for (size_t i = 0; i <= 26; ++i) {
            new_word[joker->index[0]] = i + 'a';

            // Pour les boucles qui suivent, on incrémente la lettre  de
            // manière à ne pas se retrouver avec plusieurs fois la même
            // combinaison et avoir une seule fois chaque anagrammes
            for (size_t j = 0 + i; j <= 26; ++j) {
                new_word[joker->index[1]] = j + 'a';
                word_dict_search_anagrams(self, new_word, result);
            }
        }
        break;

    // Idem pour 3 jokers
    case 3:
        for (size_t i = 0; i <= 26; ++i) {
            new_word[joker->index[0]] = i + 'a';
            for (size_t j = 0 + i; j <= 26; ++j) {
                new_word[joker->index[1]] = j + 'a';
                for (size_t k = 0 + j; k <= 26; ++k) {
                    new_word[joker->index[2]] = k + 'a';
                    word_dict_search_anagrams(self, new_word, result);
                }
            }
        }
        break;

    // Idem pour 4 jokers
    case 4:
        for (size_t i = 0; i <= 26; ++i) {
            new_word[joker->index[0]] = i + 'a';
            for (size_t j = 0 + i; j <= 26; ++j) {
                new_word[joker->index[1]] = j + 'a';
                for (size_t k = 0 + j; k <= 26; ++k) {
                    new_word[joker->index[2]] = k + 'a';
                    for (size_t l = 0 + k; l <= 26; ++l) {
                        new_word[joker->index[3]] = l + 'a';
                        word_dict_search_anagrams(self, new_word, result);
                    }
                }
            }
        }
        break;
    }
    free(joker);
    free(new_word);
}
