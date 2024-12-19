#include <stdio.h>
#include <string.h>
#include "service.h"
#include <gtk/gtk.h>
int valider_service_form(Service s) {
    // Validation de l'identifiant
    if (s.id <= 0) {
        printf("Erreur : ID invalide.\n");
        return 0;
    }

    // Validation du nom
    if (strlen(s.name) == 0) {
        printf("Erreur : Le nom du service ne peut pas être vide.\n");
        return 0;
    }

    // Validation du téléphone
    if (strlen(s.phone) < 8 || strspn(s.phone, "0123456789") != strlen(s.phone)) {
        printf("Erreur : Téléphone invalide (doit contenir uniquement des chiffres, au moins 8 caractères).\n");
        return 0;
    }

    // Validation de la catégorie
    if (strlen(s.category) == 0) {
        printf("Erreur : La catégorie ne peut pas être vide.\n");
        return 0;
    }

    // Validation de l'heure
    if (s.time_hour < 0 || s.time_hour >= 24 ||
        s.time_minute < 0 || s.time_minute >= 60 ||
        s.time_second < 0 || s.time_second >= 60) {
        printf("Erreur : Heure invalide.\n");
        return 0;
    }

    // Validation de la limite de réservation
    if (s.reservation_limit < 0) {
        printf("Erreur : Limite de réservation invalide.\n");
        return 0;
    }

    // Validation du prix
    if (s.price <= 0) {
        printf("Erreur : Prix invalide (doit être supérieur à 0).\n");
        return 0;
    }

    // Validation de la remise
    if (s.remise != 0 && s.remise != 1) {
        printf("Erreur : Valeur de remise invalide.\n");
        return 0;
    }

    // Validation des types de paiement
    if (s.payment_type[0] != 0 && s.payment_type[0] != 1) {
        printf("Erreur : Type de paiement 1 invalide.\n");
        return 0;
    }
    if (s.payment_type[1] != 0 && s.payment_type[1] != 1) {
        printf("Erreur : Type de paiement 2 invalide.\n");
        return 0;
    }

    // Validation de la description
    if (strlen(s.description) == 0) {
        printf("Erreur : La description ne peut pas être vide.\n");
        return 0;
    }

    return 1; // Tout est valide
}

int ajouter_service(char *filename, Service service) {
    FILE *f = fopen(filename, "a");  // Ouvrir le fichier en mode ajout (append)
    if (f != NULL) {
        // Écrire les informations du service dans le fichier avec le format spécifié
        fprintf(f, "%d|%s|%s|%s|%s|%d|%d|%d|%d|%.2f|%d|%d|%d|%d\n",
                service.id,              // ID du service
                service.name,            // Nom du service
                service.category,        // Catégorie du service
                service.description,     // Description du service
                service.phone,           // Numéro de téléphone
                service.time_hour,       // Heure
                service.time_minute,     // Minute
                service.time_second,     // Seconde
                service.reservation_limit, // Limite de réservation
                service.price,           // Prix
                service.remise,          // Remise (0 ou 1)
                service.payment_type[0], // Paiement par carte (0 ou 1)
                service.payment_type[1], // Paiement en espèces (0 ou 1)
                service.payment_type[2]); // Paiement par chèque (0 ou 1)

        fclose(f);  // Fermer le fichier après l'écriture
        return 1;  // Succès
    }
    return 0;  // Échec si le fichier ne peut pas être ouvert
}



int modifier_service(char *filename, int id, Service new_service) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return 0;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        fclose(f);
        printf("Erreur : Impossible de créer le fichier temporaire.\n");
        return 0;
    }

    Service service = {0};
    int found = 0;
    char line[512];

    // Lire le fichier ligne par ligne
    while (fgets(line, sizeof(line), f) != NULL) {
        // Affichage de la ligne lue pour débogage
        printf("Ligne lue : %s", line);

if (sscanf(line, "%d|%49[^|]|%49[^|]|%199[^|]|%19[^|]|%d|%d|%d|%d|%lf|%d|%d|%d|%d",
           &service.id, service.name, service.category, service.description,
           service.phone, &service.time_hour, &service.time_minute, &service.time_second,
           &service.reservation_limit, &service.price, &service.remise,
           &service.payment_type[0], &service.payment_type[1], &service.payment_type[2]) == 14) {

    printf("Service trouvé : ID=%d, Nom=%s\n", service.id, service.name);

    if (service.id == id) {
        found = 1;
        service = new_service;
    }

    fprintf(temp, "%d|%s|%s|%s|%s|%02d|%02d|%02d|%d|%.2lf|%d|%d|%d|%d\n",
            service.id, service.name, service.category, service.description,
            service.phone, service.time_hour, service.time_minute, service.time_second,
            service.reservation_limit, service.price, service.remise,
            service.payment_type[0], service.payment_type[1], service.payment_type[2]);
} else {
    printf("Erreur lors du parsing de la ligne : '%s'\n", line);
}

    }

    fclose(f);
    fclose(temp);

    // Si le service a été trouvé et modifié, nous remplaçons l'ancien fichier
    if (found) {
        remove(filename);
        rename("temp.txt", filename);  // Remplacer le fichier original
        return 1;
    } else {
        remove("temp.txt");
        printf("Erreur : Service avec l'ID %d introuvable.\n", id);
        return 0;
    }
}



int supprimer_service(const char *filename, int id) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", filename);
        return 0;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        fclose(f);
        printf("Erreur : Impossible de créer le fichier temporaire.\n");
        return 0;
    }

    Service service;
    char line[512];
    int found = 0;

    // Lire le fichier ligne par ligne
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "%d|%49[^|]|%49[^|]|%199[^|]|%19[^|]|%d|%d|%d|%d|%lf|%d|%d|%d|%d",
                   &service.id, service.name, service.category, service.description,
                   service.phone, &service.time_hour, &service.time_minute, &service.time_second,
                   &service.reservation_limit, &service.price, &service.remise,
                   &service.payment_type[0], &service.payment_type[1], &service.payment_type[2]) == 14) {
            // Vérifier si l'ID correspond
            if (service.id == id) {
                found = 1; // Marquer comme trouvé
                continue; // Ne pas écrire cette ligne dans le fichier temporaire
            }
        }

        // Écrire les lignes non supprimées dans le fichier temporaire
        fprintf(temp, "%s", line);
    }

    fclose(f);
    fclose(temp);

    if (found) {
        remove(filename);
        rename("temp.txt", filename);
        return 1;
    } else {
        remove("temp.txt");
        printf("Erreur : Service avec l'ID %d introuvable.\n", id);
        return 0;
    }
}



// Other functions...

int affecter_service(char *filename, int id) {
    int tr = 0;  // Flag to indicate success of affectation
    Service service;
    FILE *f = fopen(filename, "r");
    FILE *f_temp = fopen("temp.txt", "w");

    if (f != NULL && f_temp != NULL) {
        while (fscanf(f, "%d %s %s %s %d %d %d %d %lf %d %d %d %s",
                      &service.id, service.name, service.category, service.description,
                      &service.time_hour, &service.time_minute, &service.time_second,
                      &service.reservation_limit, &service.price, &service.remise,
                      &service.payment_type[0], &service.payment_type[1], service.phone) != EOF) {
            if (service.id == id) {
                if (service.reservation_limit > 0) {
                    service.reservation_limit -= 1;
                    tr = 1;  // Affectation successful
                }
                // Write the updated service to the temp file
                fprintf(f_temp, "%d %s %s %s %d %d %d %d %lf %d %d %d %s",
                        service.id, service.name, service.category, service.description,
                        service.time_hour, service.time_minute, service.time_second,
                        service.reservation_limit, service.price, service.remise,
                        service.payment_type[0], service.payment_type[1], service.phone);
            } else {
                // Write the unchanged service to the temp file
                fprintf(f_temp, "%d %s %s %s %d %d %d %d %lf %d %d %d %s",
                        service.id, service.name, service.category, service.description,
                        service.time_hour, service.time_minute, service.time_second,
                        service.reservation_limit, service.price, service.remise,
                        service.payment_type[0], service.payment_type[1], service.phone);
            }
        }
        fclose(f);
        fclose(f_temp);

        if (tr) {
            remove(filename);             // Delete original file
            rename("temp.txt", filename); // Rename temp file to original filename
        } else {
            remove("temp.txt"); // If no affectation, delete temp file
        }
    } else {
        if (f) fclose(f);
        if (f_temp) fclose(f_temp);
    }

    return tr; // Return success flag
}


// Function to search for a service in the file by ID
Service chercher_service(const char *filename, int id) {
    Service service;
    service.id = -1;  // Valeur par défaut si le service n'est pas trouvé
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Erreur : Impossible d'ouvrir le fichier");
        return service;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        Service temp;
        if (sscanf(line, "%d|%49[^|]|%49[^|]|%199[^|]|%14[^|]|%d|%d|%d|%d|%lf|%d|%d|%d|%d",
                   &temp.id, temp.name, temp.category, temp.description, temp.phone,
                   &temp.time_hour, &temp.time_minute, &temp.time_second,
                   &temp.reservation_limit, &temp.price,
                   &temp.remise, &temp.payment_type[0],
                   &temp.payment_type[1], &temp.payment_type[2]) == 14) {
            if (temp.id == id) {
                service = temp;
                break;
            }
        }
    }

    fclose(file);
    return service;
}



reserv chercher_reserv(const char *filename, int id) {
    reserv r;
    r.id = -1;  // Valeur par défaut si le service n'est pas trouvé

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur : Impossible d'ouvrir le fichier");
        return r;
    }

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        reserv temp; // Utilisation cohérente de la même structure
        if (sscanf(line, "%d|%49[^|]|%49[^|]|%199[^|]|%14[^|]",
                   &temp.id, temp.nom, temp.categorie, temp.description, temp.telephone) == 5) {
            if (temp.id == id) {
                r = temp;
                break;
            }
        }
    }

    fclose(file);
    return r;
}


// Fonction pour ajouter un service dans le TreeView
void afficher_service_dans_treeview(Service service, GtkTreeView *treeview) {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
    GtkTreeIter iter;

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
                       0, service.id,
                       1, service.name,
                       2, service.category,
                       3, service.description,
                       4, service.phone,
                       5, service.time_hour,
                       6, service.time_minute,
                       7, service.time_second,
                       8, service.reservation_limit,
                       9, service.price,
                       10, service.remise,
                       11, service.payment_type[0],
                       12, service.payment_type[1],
                       13, service.payment_type[2],
                       -1);
}


// Fonction pour configurer les colonnes du TreeView
void configurer_columns_treeview(GtkTreeView *treeview) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Colonne ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Nom
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Catégorie
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Catégorie", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Description
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Description", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Téléphone
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Téléphone", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Heure
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Heure", renderer, "text", 5, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Minute
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Minute", renderer, "text", 6, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Seconde
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Seconde", renderer, "text", 7, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Limite de réservation
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Limite Réservation", renderer, "text", 8, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Prix
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Prix", renderer, "text", 9, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Remise
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Remise", renderer, "text", 10, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Types de paiement
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Paiement 1", renderer, "text", 11, NULL);
    gtk_tree_view_append_column(treeview, column);

    column = gtk_tree_view_column_new_with_attributes("Paiement 2", renderer, "text", 12, NULL);
    gtk_tree_view_append_column(treeview, column);

    column = gtk_tree_view_column_new_with_attributes("Paiement 3", renderer, "text", 13, NULL);
    gtk_tree_view_append_column(treeview, column);
}

// Fonction pour initialiser le modèle du TreeView
GtkListStore *creer_model_treeview() {
    GtkListStore *store = gtk_list_store_new(14, 
        G_TYPE_INT,       // ID
        G_TYPE_STRING,    // Nom
        G_TYPE_STRING,    // Catégorie
        G_TYPE_STRING,    // Description
        G_TYPE_STRING,    // Téléphone
        G_TYPE_INT,       // Heure
        G_TYPE_INT,       // Minute
        G_TYPE_INT,       // Seconde
        G_TYPE_INT,       // Limite Réservation
        G_TYPE_DOUBLE,    // Prix
        G_TYPE_INT,       // Remise
        G_TYPE_INT,       // Paiement 1
        G_TYPE_INT,       // Paiement 2
        G_TYPE_INT        // Paiement 3
    );
    return store;
}
void rechercher_service_par_categorie(char *filename, const char *category, GtkTreeView *treeview) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        g_print("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return;
    }

    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
    gtk_list_store_clear(store); // Vider le TreeView avant d'ajouter de nouvelles données

    char line[512];
    gboolean found = FALSE;

    while (fgets(line, sizeof(line), file)) {
        Service temp;
        // Lire une ligne et la convertir en structure Service
        if (sscanf(line, "%d|%49[^|]|%49[^|]|%199[^|]|%14[^|]|%d|%d|%d|%d|%lf|%d|%d|%d|%d",
                   &temp.id, temp.name, temp.category, temp.description, temp.phone,
                   &temp.time_hour, &temp.time_minute, &temp.time_second,
                   &temp.reservation_limit, &temp.price,
                   &temp.remise, &temp.payment_type[0],
                   &temp.payment_type[1], &temp.payment_type[2]) == 14) {
            // Comparer la catégorie
            if (strcmp(temp.category, category) == 0) {
                afficher_service_dans_treeview(temp, treeview); // Ajouter au TreeView
                found = TRUE;
            }
        }
    }

    fclose(file);

    if (!found) {
        g_print("Aucun service trouvé pour la catégorie : %s\n", category);
    }
}void setup_treeview(GtkTreeView *treeview) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    // Colonne ID
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Nom
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Catégorie
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Catégorie", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(treeview, column);

    // Colonne Description
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Description", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(treeview, column);
}





