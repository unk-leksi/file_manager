#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "os_file.h"
#pragma warning(disable:4996)

struct title_dir;

typedef struct title_file
{
    int size_file;
    title_dir* parent_of_file;
    char* name_file;
}title_file;

typedef struct title_dir
{
    char* name_dir;
    title_dir* parent_of_dir;
    title_dir** children_of_dir;
    int count_of_child;
    title_file** child_of_files;
    int count_of_files;
}title_dir;

title_dir* root = NULL;
title_dir* cur_dir = NULL;
int size_mem;
title_dir* runner = NULL;

int check_name(char* name)
{
    int k = 0, counter = 0;
    if (name[0] == '.' && strlen(name) > 1 && name[1] != '\0')
    {
        if (strcmp(name, "..") == 0)
        {
            return 1;
        }
        return 0;
    }

    while (name[k] != '\0') {
        counter++;
        k++;
    }
    if (counter >= 33) {
        return 0;
    }
    return 1;
}

int check_new_node(char* name)
{
    for (int i = 0; i < runner->count_of_child; i++) {
        if (strcmp(runner->children_of_dir[i]->name_dir, name) == 0) {
            return 0;
        }
    }
    for (int i = 0; i < runner->count_of_files; i++) {
        if (strcmp(runner->child_of_files[i]->name_file, name) == 0) {
            return 0;
        }
    }
    return 1;
}

char** slices_of_titles(const char* path, int* size)
{
    int rows = 0, count = 0, i = 0, max = 0, count_letter = 1;
    while (path[i] != '\0') {
        if (path[i] == '/' && path[i + 1] == '/') {
            return NULL;
        }
        if (path[i] == '/') {
            count_letter = 1;
            count++;
        }
        if (path[i] != '/') {
            count_letter++;
        }
        if (max < count_letter) {
            max = count_letter;
        }
        i++;
    }
    if (max < count_letter) {
        max = count_letter;
    }

    if (path[0] == '/') {
        runner = root;
        rows = count;
    }
    else {
        runner = cur_dir;
        rows = count + 1;
    }
    if (path[i - 1] == '/') {
        rows--;
    }

    char** slices = (char**)malloc(sizeof(char*) * rows);
    if (slices) {
        for (int i = 0; i < rows; i++) {
            slices[i] = (char*)malloc(sizeof(char) * max);
        }
    }

    int k = 0, j = 0;
    i = 0;
    if (path[0] == '/') {
        k = 1;
    }

    while (path[k] != '\0') {
        if (path[k] == '/') {
            i++;
            if (i != 0) {
                slices[i - 1][j] = '\0';
            }
            j = 0;
            k++;
        }
        else {
            slices[i][j] = path[k];
            j++;
            k++;
        }
    }
    if (j != 0) {
        slices[rows - 1][j] = '\0';
    }
    *size = rows;
    return slices;
}

void find_parent(char** names, int count_rows)
{
    int flag = 0;
    if (strcmp(names[0], "..") == 0 && cur_dir == root)
    {
        runner = NULL;
        return;
    }

    for (int i = 0; i < count_rows - 1; i++) {
        flag = 0;
        if (strcmp(names[i], ".") == 0)
        {
            flag = 1;
            i++;
        }
        else if (strcmp(names[i], "..") == 0)
        {
            if (runner->parent_of_dir == NULL)
            {
                runner = NULL;
                return;
            }
            else {
                flag = 1;
                runner = runner->parent_of_dir;
                i++;
            }
        }

        if (strcmp(names[i], ".") == 0 && runner->count_of_child == 0)
        {
            i--;
        }
        else if (strcmp(names[i], "..") == 0 && runner->count_of_child == 0)
        {
            i--;
        }

        if (i != count_rows - 1) {
            for (int j = 0; j < runner->count_of_child; j++) {
                flag = 0;
                if (strcmp(names[i], ".") == 0 || strcmp(names[i], "..") == 0)
                {
                    flag = 1;
                    i--;
                    break;
                }
                else if (strcmp(names[i], runner->children_of_dir[j]->name_dir) == 0 && i != count_rows - 1)
                {
                    flag = 1;
                    runner = runner->children_of_dir[j];
                    break;
                }
            }
        }
        if (flag == 0)
        {
            runner = NULL;
            break;
        }
    }
}

void delete_dir(title_dir* c_dir)
{
    if (c_dir->count_of_files > 0) {
        while (c_dir->count_of_files > 0) {
            size_mem = size_mem + c_dir->child_of_files[c_dir->count_of_files - 1]->size_file;
            c_dir->child_of_files[c_dir->count_of_files - 1]->size_file = 0;
            c_dir->child_of_files[c_dir->count_of_files - 1]->parent_of_file = NULL;
            free(c_dir->child_of_files[c_dir->count_of_files - 1]->name_file);
            free(c_dir->child_of_files[c_dir->count_of_files - 1]);
            c_dir->count_of_files--;
        }
        free(c_dir->child_of_files);
    }

    if (c_dir->count_of_child > 0) {
        for (int i = 0; i < c_dir->count_of_child; i++) {
            c_dir->children_of_dir[i]->parent_of_dir = NULL;
            free(c_dir->children_of_dir[i]->name_dir);
            delete_dir(c_dir->children_of_dir[i]);
            free(c_dir->children_of_dir[i]);
        }
        free(c_dir->children_of_dir);
        c_dir->count_of_child = 0;
    }
}

void cleaning_array(char** slices, int count_rows)
{
    for (int m = 0; m < count_rows; m++)
    {
        free(slices[m]);
    }
    free(slices);
}

int my_create(int disk_size)
{
    if (root == NULL && size_mem == 0)
    {
        root = (title_dir*)malloc(sizeof(title_dir) * 1);
        if (root) {
            root->name_dir = (char*)malloc(sizeof(char) * 2);
            root->name_dir[0] = '/';
            root->name_dir[1] = '\0';
            root->children_of_dir = NULL;
            root->count_of_child = 0;
            root->parent_of_dir = NULL;
            root->child_of_files = NULL;
            root->count_of_files = 0;
            cur_dir = root;
            size_mem = disk_size;
            return 1;
        }
    }
    return 0;
}

int my_destroy()
{
    if (root == NULL)
    {
        return 0;
    }
    else
    {
        delete_dir(root);
        if (root->count_of_child == 0 && root->count_of_files == 0)
        {
            size_mem = 0;
            free(root->name_dir);
            free(root);
            root = NULL;
            return 1;
        }
    }
    return 0;
}

int my_create_dir(const char* path)
{
    if (root == NULL)
    {
        return 0;
    }
    int count_rows = 0;
    char** slices_titles = slices_of_titles(path, &count_rows);
    if (slices_titles)
    {
        if (strcmp(slices_titles[count_rows - 1], ".") == 0 || strcmp(slices_titles[count_rows - 1], "..") == 0)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }
        find_parent(slices_titles, count_rows);

        if (runner == NULL)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }
        if (check_name(slices_titles[count_rows - 1]) == 0 || check_new_node(slices_titles[count_rows - 1]) == 0)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }
        title_dir* new_directory = (title_dir*)malloc(sizeof(title_dir) * 1);
        if (new_directory)
        {
            new_directory->parent_of_dir = runner;
            new_directory->parent_of_dir->count_of_child++;
            title_dir** new_dir_check = (title_dir**)realloc(new_directory->parent_of_dir->children_of_dir, sizeof(title_dir*) * (new_directory->parent_of_dir->count_of_child));
            if (new_dir_check != NULL) {
                new_directory->parent_of_dir->children_of_dir = new_dir_check;
            }
            else {
                cleaning_array(slices_titles, count_rows);
                return 0;
            }
            new_directory->parent_of_dir->children_of_dir[new_directory->parent_of_dir->count_of_child - 1] = new_directory;
            new_directory->name_dir = (char*)malloc(sizeof(char) * 32);
            strcpy(new_directory->name_dir, slices_titles[count_rows - 1]);
            new_directory->children_of_dir = NULL;
            new_directory->count_of_child = 0;
            new_directory->child_of_files = NULL;
            new_directory->count_of_files = 0;
            cleaning_array(slices_titles, count_rows);
            return 1;
        }
    }
    cleaning_array(slices_titles, count_rows);
    return 0;
}

int my_create_file(const char* path, int file_size)
{
    if (size_mem < file_size || root == NULL)
    {
        return 0;
    }
    int count_rows;
    char** slices_titles = slices_of_titles(path, &count_rows);
    if (slices_titles) {
        if (strcmp(slices_titles[count_rows - 1], ".") == 0 || strcmp(slices_titles[count_rows - 1], "..") == 0)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }
        find_parent(slices_titles, count_rows);
        if (runner == NULL)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }
        if (check_name(slices_titles[count_rows - 1]) == 0 || check_new_node(slices_titles[count_rows - 1]) == 0)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }
        title_file* new_file = (title_file*)malloc(sizeof(title_file) * 1);
        if (new_file)
        {
            new_file->parent_of_file = runner;
            new_file->parent_of_file->count_of_files++;
            title_file** new_file_check = (title_file**)realloc(new_file->parent_of_file->child_of_files, sizeof(title_file*) * (new_file->parent_of_file->count_of_files));
            if (new_file_check != NULL) {
                new_file->parent_of_file->child_of_files = new_file_check;
            }
            else {
                cleaning_array(slices_titles, count_rows);
                return 0;
            }
            new_file->parent_of_file->child_of_files[new_file->parent_of_file->count_of_files - 1] = new_file;
            new_file->name_file = (char*)malloc(sizeof(char) * 32);
            strcpy(new_file->name_file, slices_titles[count_rows - 1]);
            size_mem = size_mem - file_size;
            new_file->size_file = file_size;
            cleaning_array(slices_titles, count_rows);
            return 1;
        }
    }
    cleaning_array(slices_titles, count_rows);
    return 0;
}

int my_remove(const char* path, int recursive)
{
    int count_rows = 0, flag_exist = 0;
    char** slices_titles = slices_of_titles(path, &count_rows);
    if (slices_titles)
    {
        find_parent(slices_titles, count_rows);
        if (runner == NULL)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }
        if (check_name(slices_titles[count_rows - 1]) == 0)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }

        if (strcmp(slices_titles[count_rows - 1], ".") == 0)
        {
            flag_exist = 1;
        }

        if (strcmp(slices_titles[count_rows - 1], "..") == 0)
        {
            runner = runner->parent_of_dir;
            flag_exist = 1;
        }

        if (flag_exist == 1 && recursive != 0) {
            if (strcmp(cur_dir->name_dir, runner->name_dir) == 0)
            {
                cur_dir = root; //if cur_dir was deleted
            }
            delete_dir(runner);
            if (runner->count_of_child == 0 && runner->count_of_files == 0)
            {
                free(runner->name_dir);
                for (int i = 0; i < runner->parent_of_dir->count_of_child; i++) {
                    if (strcmp(runner->name_dir, runner->parent_of_dir->children_of_dir[i]->name_dir) == 0) {
                        free(runner->parent_of_dir->children_of_dir[i]->name_dir);
                        if ((runner->parent_of_dir->count_of_child - 1) == i) {
                            free(runner->parent_of_dir->children_of_dir[i]);
                            runner->parent_of_dir->count_of_child--;
                            runner->parent_of_dir->children_of_dir = (title_dir**)realloc(runner->parent_of_dir->children_of_dir, sizeof(title_dir*) * (runner->parent_of_dir->count_of_child));
                        }
                        else {
                            title_dir* tmp = NULL;
                            tmp = runner->parent_of_dir->children_of_dir[i];
                            runner->parent_of_dir->children_of_dir[i] = runner->parent_of_dir->children_of_dir[runner->count_of_child - 1];
                            runner->parent_of_dir->children_of_dir[runner->count_of_child - 1] = tmp;
                            free(runner->parent_of_dir->children_of_dir[runner->count_of_child - 1]);
                            runner->parent_of_dir->count_of_child--;
                            runner->parent_of_dir->children_of_dir = (title_dir**)realloc(runner->parent_of_dir->children_of_dir, sizeof(title_dir*) * (runner->parent_of_dir->count_of_child));
                        }
                        runner->parent_of_dir = NULL;
                        cleaning_array(slices_titles, count_rows);
                        return 1;
                    }
                }
            }
            cleaning_array(slices_titles, count_rows);
            return 0;
        }

        if (runner->count_of_files > 0) {
            for (int i = 0; i < runner->count_of_files; i++) {
                if (strcmp(slices_titles[count_rows - 1], runner->child_of_files[i]->name_file) == 0) {
                    runner->child_of_files[i]->parent_of_file = NULL;
                    free(runner->child_of_files[i]->name_file);
                    size_mem = size_mem + runner->child_of_files[i]->size_file;
                    runner->child_of_files[i]->size_file = 0;
                    if ((runner->count_of_files - 1) == i) {
                        free(runner->child_of_files[i]);
                        runner->count_of_files--;
                        runner->child_of_files = (title_file**)realloc(runner->child_of_files, sizeof(title_file*) * (runner->count_of_files));
                    }
                    else {
                        title_file* tmp = NULL;
                        tmp = runner->child_of_files[i];
                        runner->child_of_files[i] = runner->child_of_files[runner->count_of_files - 1];
                        runner->child_of_files[runner->count_of_files - 1] = tmp;
                        free(runner->child_of_files[runner->count_of_files - 1]);
                        runner->count_of_files--;
                        runner->child_of_files = (title_file**)realloc(runner->child_of_files, sizeof(title_file*) * (runner->count_of_files));
                    }
                    cleaning_array(slices_titles, count_rows);
                    return 1;
                }
            }
        }

        if (runner->count_of_child > 0) {
            for (int i = 0; i < runner->count_of_child; i++) {
                if (strcmp(slices_titles[count_rows - 1], runner->children_of_dir[i]->name_dir) == 0) {
                    if (runner->children_of_dir[i]->count_of_child != 0 && recursive == 0)
                    {
                        cleaning_array(slices_titles, count_rows);
                        return 0;
                    }
                    else if (runner->children_of_dir[i]->count_of_files != 0 && recursive == 0)
                    {
                        cleaning_array(slices_titles, count_rows);
                        return 0;
                    }
                    else
                    {
                        if (strcmp(cur_dir->name_dir, runner->children_of_dir[i]->name_dir) == 0)
                        {
                            cur_dir = root; //if cur_dir was deleted
                        }
                        delete_dir(runner->children_of_dir[i]);
                        runner->children_of_dir[i]->parent_of_dir = NULL;
                        if (runner->children_of_dir[i]->count_of_child == 0 && runner->children_of_dir[i]->count_of_files == 0)
                        {
                            free(runner->children_of_dir[i]->name_dir);
                            if ((runner->count_of_child - 1) == i) {
                                free(runner->children_of_dir[i]);
                                runner->count_of_child--;
                                runner->children_of_dir = (title_dir**)realloc(runner->children_of_dir, sizeof(title_dir*) * (runner->count_of_child));
                            }
                            else {
                                title_dir* tmp = NULL;
                                tmp = runner->children_of_dir[i];
                                runner->children_of_dir[i] = runner->children_of_dir[runner->count_of_child - 1];
                                runner->children_of_dir[runner->count_of_child - 1] = tmp;
                                free(runner->children_of_dir[runner->count_of_child - 1]);
                                runner->count_of_child--;
                                runner->children_of_dir = (title_dir**)realloc(runner->children_of_dir, sizeof(title_dir*) * (runner->count_of_child));
                            }
                            cleaning_array(slices_titles, count_rows);
                            return 1;
                        }
                    }
                }
            }
        }
    }
    cleaning_array(slices_titles, count_rows);
    return 0;
}

int my_change_dir(const char* path)
{
    int count_rows = 0;
    int flag1 = 0;
    char** slices_titles = slices_of_titles(path, &count_rows);
    if (slices_titles) {
        if (strcmp(slices_titles[count_rows - 1], root->name_dir) == 0)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }
        find_parent(slices_titles, count_rows);
        if (runner == NULL)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }
        if (check_name(slices_titles[count_rows - 1]) == 0)
        {
            cleaning_array(slices_titles, count_rows);
            return 0;
        }

        if (strcmp(slices_titles[count_rows - 1], ".") == 0)
        {
            cur_dir = runner;
            cleaning_array(slices_titles, count_rows);
            return 1;
        }

        if (strcmp(slices_titles[count_rows - 1], "..") == 0)
        {
            cur_dir = runner->parent_of_dir;
            cleaning_array(slices_titles, count_rows);
            return 1;
        }

        if (runner->count_of_files > 0) {
            for (int i = 0; i < runner->count_of_files; i++) {
                if (strcmp(runner->child_of_files[i]->name_file, slices_titles[count_rows - 1]) == 0)
                {
                    cleaning_array(slices_titles, count_rows);
                    return 0;
                }
            }
        }

        if (runner->count_of_child > 0) {
            for (int i = 0; i < runner->count_of_child; i++) {
                if (strcmp(runner->children_of_dir[i]->name_dir, slices_titles[count_rows - 1]) == 0)
                {
                    cur_dir = runner->children_of_dir[i];
                    cleaning_array(slices_titles, count_rows);
                    return 1;
                }
            }
        }
    }
    cleaning_array(slices_titles, count_rows);
    return 0;
}

void find_cur_path(title_dir* c_dir, char* dst)
{
    if (c_dir == root) {
        return;
    }
    find_cur_path(c_dir->parent_of_dir, dst);
    strcat(dst, c_dir->parent_of_dir->name_dir);
    if (c_dir->parent_of_dir != root)
    {
        strcat(dst, "/");
    }
}

void my_get_cur_dir(char* dst)
{
    if (cur_dir == root)
    {
        strcpy(dst, "/");
    }
    else
    {
        dst[0] = '\0';
        find_cur_path(cur_dir, dst);
        strcat(dst, cur_dir->name_dir);
    }
}

int recurs_counter_files(title_dir* c_dir, int* count_all_files)
{
    if (c_dir->count_of_files > 0)
    {
        *count_all_files = *count_all_files + (c_dir->count_of_files);
    }
    if (c_dir->count_of_child > 0)
    {
        for (int i = 0; i < c_dir->count_of_child; i++) {
            recurs_counter_files(c_dir->children_of_dir[i], count_all_files);
        }
    }
    return *count_all_files;
}

int my_files_count(const char* path)
{
    int count_all_files = 0, count_rows = 0, flag_exist = 0;
    char** slices_titles = slices_of_titles(path, &count_rows);
    if (slices_titles)
    {
        find_parent(slices_titles, count_rows);
        if (runner == NULL)
        {
            cleaning_array(slices_titles, count_rows);
            return -1;
        }
        if (check_name(slices_titles[count_rows - 1]) == 0)
        {
            cleaning_array(slices_titles, count_rows);
            return -1;
        }

        if (strcmp(slices_titles[count_rows - 1], ".") == 0)
        {
            flag_exist = 1;
        }

        if (strcmp(slices_titles[count_rows - 1], "..") == 0)
        {
            runner = runner->parent_of_dir;
            flag_exist = 1;
        }

        if (flag_exist == 1)
        {
            recurs_counter_files(runner, &count_all_files);
            cleaning_array(slices_titles, count_rows);
            return count_all_files;
        }

        if (runner->count_of_files > 0) {
            for (int i = 0; i < runner->count_of_files; i++) {
                if (strcmp(slices_titles[count_rows - 1], runner->child_of_files[i]->name_file) == 0) {
                    count_all_files++;
                    cleaning_array(slices_titles, count_rows);
                    return count_all_files;
                }
            }
        }
        if (runner->count_of_child > 0) {
            for (int i = 0; i < runner->count_of_child; i++) {
                if (strcmp(slices_titles[count_rows - 1], runner->children_of_dir[i]->name_dir) == 0) {
                    recurs_counter_files(runner->children_of_dir[i], &count_all_files);
                    cleaning_array(slices_titles, count_rows);
                    return count_all_files;
                }
            }
        }
    }
    cleaning_array(slices_titles, count_rows);
    return -1;
}

void setup_file_manager(file_manager_t* fm)
{
    fm->create = my_create;
    fm->destroy = my_destroy;
    fm->create_dir = my_create_dir;
    fm->create_file = my_create_file;
    fm->remove = my_remove;
    fm->change_dir = my_change_dir;
    fm->get_cur_dir = my_get_cur_dir;
    fm->files_count = my_files_count;
}
