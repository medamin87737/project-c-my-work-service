#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "service.h"
void on_button30_clicked(GtkButton *button, gpointer user_data) {
    // Déclaration des widgets GTK
    GtkWidget *entry_id, *entry_name, *entry_phone;
    GtkWidget *spin_hour, *spin_minute, *spin_second;
    GtkWidget *spin_reservation, *spin_price;
    GtkWidget *combobox_category, *textview_description;
    GtkWidget *radio_yes, *radio_no;
    GtkWidget *check_carte, *check_espece, *check_check;
    GtkWidget *label_status;

    // Déclaration de la structure Service
    Service s;

    // Nom du fichier
    char filename[] = "parkings.txt";

    // Récupération des widgets via lookup_widget
    entry_id = lookup_widget(GTK_WIDGET(button), "entry43");
    entry_name = lookup_widget(GTK_WIDGET(button), "entry42");
    entry_phone = lookup_widget(GTK_WIDGET(button), "entry45");
    spin_hour = lookup_widget(GTK_WIDGET(button), "spinbutton28");
    spin_minute = lookup_widget(GTK_WIDGET(button), "spinbutton29");
    spin_second = lookup_widget(GTK_WIDGET(button), "spinbutton30");
    spin_reservation = lookup_widget(GTK_WIDGET(button), "spinbutton31");
    spin_price = lookup_widget(GTK_WIDGET(button), "spinbutton47");
    combobox_category = lookup_widget(GTK_WIDGET(button), "comboboxentry9");
    textview_description = lookup_widget(GTK_WIDGET(button), "textview8");
    radio_yes = lookup_widget(GTK_WIDGET(button), "radiobutton31");
    radio_no = lookup_widget(GTK_WIDGET(button), "radiobutton32");
    check_carte = lookup_widget(GTK_WIDGET(button), "checkbutton23");
    check_espece = lookup_widget(GTK_WIDGET(button), "checkbutton24");
    check_check = lookup_widget(GTK_WIDGET(button), "checkbutton42");
    label_status = lookup_widget(GTK_WIDGET(button), "label650");

    // Récupération des textes entrés par l'utilisateur
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const char *name_text = gtk_entry_get_text(GTK_ENTRY(entry_name));
    const char *phone_text = gtk_entry_get_text(GTK_ENTRY(entry_phone));
    const char *category_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combobox_category));

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_description));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    char *description_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
// Vérifier si l'ID existe déjà dans le fichier
Service existing_service = chercher_service("parkings.txt", atoi(id_text));
if (existing_service.id != -1) { // Si l'ID existe déjà
    gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Un service avec cet ID existe déjà.");
    g_free(description_text);
    return;
}

    // Validation des champs
    if (strlen(id_text) == 0 || strlen(name_text) == 0 || strlen(phone_text) == 0 ||
        strlen(category_text) == 0 || strlen(description_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Tous les champs doivent être remplis.");
        g_free(description_text);
        return;
    }

    // Vérification que le name commence par une majuscule
    if (!g_ascii_isalpha(name_text[0]) || !g_ascii_isupper(name_text[0])) {
        char corrected_name[256];
        strcpy(corrected_name, name_text);
        corrected_name[0] = g_ascii_toupper(corrected_name[0]); // Convertir la première lettre en majuscule
        gtk_entry_set_text(GTK_ENTRY(entry_name), corrected_name); // Mettre à jour le champ avec le texte corrigé
        gtk_label_set_text(GTK_LABEL(label_status), "Le champ 'Nom' a été corrigé pour commencer par une majuscule.");
        g_free(description_text);
        return;
    }

    // Vérification que l'ID est un entier positif
    if (!g_ascii_isdigit(id_text[0]) || atoi(id_text) <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : L'ID doit être un entier positif.");
        g_free(description_text);
        return;
    }

    // Vérification du format du numéro de téléphone (8 chiffres)
    if (strlen(phone_text) != 8 || strspn(phone_text, "012345678") != 8) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Le numéro de téléphone doit contenir exactement 8 chiffres.");
        g_free(description_text);
        return;
    }

    // Vérification des valeurs des spin buttons
    int hour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_hour));
    int minute = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_minute));
    int second = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_second));
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : L'heure, les minutes et les secondes doivent être valides.");
        g_free(description_text);
        return;
    }

    // Vérification du prix et de la limite de réservation
    int price = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_price));
    int reservation = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_reservation));
    if (price <= 0 || reservation <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Le prix et la limite de réservation doivent être positifs.");
        g_free(description_text);
        return;
    }

    // Vérification qu'au moins un mode de paiement est sélectionné
    gboolean payment_selected = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_carte)) ||
                                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_espece)) ||
                                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_check));
    if (!payment_selected) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Au moins un mode de paiement doit être sélectionné.");
        g_free(description_text);
        return;
    }

    // Remplir la structure Service
    s.id = atoi(id_text);
    strcpy(s.name, name_text);
    strcpy(s.phone, phone_text);
    strcpy(s.category, category_text);
    strcpy(s.description, description_text);
    g_free(description_text);

    s.time_hour = hour;
    s.time_minute = minute;
    s.time_second = second;
    s.reservation_limit = reservation;
    s.price = price;
    s.remise = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_yes)) ? 1 : 0;

    s.payment_type[0] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_carte)) ? 1 : 0;
    s.payment_type[1] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_espece)) ? 1 : 0;
    s.payment_type[2] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_check)) ? 1 : 0;

    // Appeler la fonction ajouter_service
    if (ajouter_service(filename, s)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Service ajouté avec succès !");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Impossible d'ajouter le service.");
    }

    // Réinitialisation des champs
    gtk_entry_set_text(GTK_ENTRY(entry_id), "");
    gtk_entry_set_text(GTK_ENTRY(entry_name), "");
    gtk_entry_set_text(GTK_ENTRY(entry_phone), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hour), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_minute), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_second), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_reservation), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_price), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_category), -1);
    gtk_text_buffer_set_text(buffer, "", -1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_yes), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_no), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_carte), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_espece), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_check), FALSE);
}

void
on_button31_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{

    // Déclaration des widgets GTK
    GtkWidget *entry_id, *entry_name, *entry_phone;
    GtkWidget *spin_hour, *spin_minute, *spin_second;
    GtkWidget *spin_reservation, *spin_price;
    GtkWidget *combobox_category, *textview_description;
    GtkWidget *radio_yes, *radio_no;
    GtkWidget *check_carte, *check_espece,*check_check;

    // Récupération des widgets via lookup_widget
    entry_id = lookup_widget(GTK_WIDGET(button), "entry43");
    entry_name = lookup_widget(GTK_WIDGET(button), "entry42");
    entry_phone = lookup_widget(GTK_WIDGET(button), "entry45");
    spin_hour = lookup_widget(GTK_WIDGET(button), "spinbutton28");
    spin_minute = lookup_widget(GTK_WIDGET(button), "spinbutton29");
    spin_second = lookup_widget(GTK_WIDGET(button), "spinbutton30");
    spin_reservation = lookup_widget(GTK_WIDGET(button), "spinbutton31");
    spin_price = lookup_widget(GTK_WIDGET(button), "spinbutton47");
    combobox_category = lookup_widget(GTK_WIDGET(button), "comboboxentry9");
    textview_description = lookup_widget(GTK_WIDGET(button), "textview8");
    radio_yes = lookup_widget(GTK_WIDGET(button), "radiobutton31");
    radio_no = lookup_widget(GTK_WIDGET(button), "radiobutton32");
    check_carte = lookup_widget(GTK_WIDGET(button), "checkbutton23");
    check_espece = lookup_widget(GTK_WIDGET(button), "checkbutton24");
    check_check = lookup_widget(GTK_WIDGET(button), "checkbutton42");
    // Réinitialiser les champs du formulaire
    gtk_entry_set_text(GTK_ENTRY(entry_id), "");
    gtk_entry_set_text(GTK_ENTRY(entry_name), "");
    gtk_entry_set_text(GTK_ENTRY(entry_phone), "");

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hour), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_minute), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_second), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_reservation), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_price), 0);

    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_category), -1);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_description));
    gtk_text_buffer_set_text(buffer, "", -1);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_yes), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_no), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_carte), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_espece), FALSE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_check), FALSE);
}

void on_button105_clicked(GtkButton *button, gpointer user_data) {
    // Récupération des widgets
    GtkWidget *entry_id = lookup_widget(GTK_WIDGET(button), "entry96");
    GtkWidget *entry_name = lookup_widget(GTK_WIDGET(button), "entry97");
    GtkWidget *entry_phone = lookup_widget(GTK_WIDGET(button), "entry98");
    GtkWidget *spin_hour = lookup_widget(GTK_WIDGET(button), "spinbutton71");
    GtkWidget *spin_minute = lookup_widget(GTK_WIDGET(button), "spinbutton72");
    GtkWidget *spin_second = lookup_widget(GTK_WIDGET(button), "spinbutton73");
    GtkWidget *spin_reservation = lookup_widget(GTK_WIDGET(button), "spinbutton74");
    GtkWidget *spin_price = lookup_widget(GTK_WIDGET(button), "spinbutton75");
    GtkWidget *combobox_category = lookup_widget(GTK_WIDGET(button), "comboboxentry23");
    GtkWidget *textview_description = lookup_widget(GTK_WIDGET(button), "textview12");
    GtkWidget *radio_yes = lookup_widget(GTK_WIDGET(button), "radiobutton64");
    GtkWidget *radio_no = lookup_widget(GTK_WIDGET(button), "radiobutton65");
    GtkWidget *check_carte = lookup_widget(GTK_WIDGET(button), "checkbutton43");
    GtkWidget *check_espece = lookup_widget(GTK_WIDGET(button), "checkbutton44");
    GtkWidget *check_autre = lookup_widget(GTK_WIDGET(button), "checkbutton45");
    GtkWidget *label_status = lookup_widget(GTK_WIDGET(button), "label651");

    if (!entry_id || !label_status) {
        g_print("Erreur : Widgets non trouvés.\n");
        return;
    }

    // Vérification de l'ID
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    if (!id_text || strlen(id_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Veuillez entrer un ID.");
        return;
    }

    int id = atoi(id_text);
    if (id <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : L'ID doit être un entier positif.");
        return;
    }

    // Recherche du service
    Service service = chercher_service("parkings.txt", id);
    if (service.id == -1) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Service introuvable.");
        return;
    }

    // Remplissage des champs
    gtk_entry_set_text(GTK_ENTRY(entry_name), service.name);
    gtk_entry_set_text(GTK_ENTRY(entry_phone), service.phone);

    // Mise à jour du combobox pour la catégorie
// Mise à jour du combobox pour la catégorie
if (combobox_category && GTK_IS_COMBO_BOX(combobox_category)) {
    GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox_category));
    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
    gboolean category_found = FALSE;

    while (valid) {
        gchar *category_text = NULL;
        gtk_tree_model_get(model, &iter, 0, &category_text, -1); // Obtenir le texte de la catégorie

        // Supprimer les espaces de début et de fin pour éviter des problèmes de comparaison
        g_strstrip(category_text);
        g_strstrip(service.category); // Optionnel : retirer aussi les espaces de la catégorie du service si nécessaire

        g_print("Comparaison : '%s' avec '%s'\n", category_text, service.category);

        if (category_text && strcmp(category_text, service.category) == 0) {
            gtk_combo_box_set_active_iter(GTK_COMBO_BOX(combobox_category), &iter); // Sélectionner l'élément correspondant
            category_found = TRUE;
            g_free(category_text);
            break;
        }

        g_free(category_text);
        valid = gtk_tree_model_iter_next(model, &iter);
    }

    if (!category_found) {
        g_print("Erreur : Catégorie '%s' introuvable dans le combobox.\n", service.category);
    }
} else {
    g_print("Erreur : Widget combobox_category invalide ou non initialisé.\n");
}


    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_description));
    gtk_text_buffer_set_text(buffer, service.description, -1);

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hour), service.time_hour);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_minute), service.time_minute);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_second), service.time_second);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_reservation), service.reservation_limit);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_price), service.price);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_yes), service.remise == 1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_no), service.remise == 0);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_carte), service.payment_type[0] == 1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_espece), service.payment_type[1] == 1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_autre), service.payment_type[2] == 1);

    gtk_label_set_text(GTK_LABEL(label_status), "Service trouvé et informations mises à jour.");
}


void on_button82_clicked(GtkButton *button, gpointer user_data) {
    // Déclarations des widgets GTK
    GtkWidget *entry_id, *entry_name, *entry_phone;
    GtkWidget *spin_hour, *spin_minute, *spin_second;
    GtkWidget *spin_reservation, *spin_price;
    GtkWidget *combobox_category, *textview_description;
    GtkWidget *radio_yes, *radio_no;
    GtkWidget *check_carte, *check_espece, *check_autre;
    GtkWidget *label_status;

    Service service_modifie;
    char filename[] = "parkings.txt";

    // Récupération des widgets
    entry_id = lookup_widget(GTK_WIDGET(button), "entry96");
    entry_name = lookup_widget(GTK_WIDGET(button), "entry97");
    entry_phone = lookup_widget(GTK_WIDGET(button), "entry98");
    spin_hour = lookup_widget(GTK_WIDGET(button), "spinbutton71");
    spin_minute = lookup_widget(GTK_WIDGET(button), "spinbutton72");
    spin_second = lookup_widget(GTK_WIDGET(button), "spinbutton73");
    spin_reservation = lookup_widget(GTK_WIDGET(button), "spinbutton74");
    spin_price = lookup_widget(GTK_WIDGET(button), "spinbutton75");
    combobox_category = lookup_widget(GTK_WIDGET(button), "comboboxentry23");
    textview_description = lookup_widget(GTK_WIDGET(button), "textview12");
    radio_yes = lookup_widget(GTK_WIDGET(button), "radiobutton64");
    radio_no = lookup_widget(GTK_WIDGET(button), "radiobutton65");
    check_carte = lookup_widget(GTK_WIDGET(button), "checkbutton43");
    check_espece = lookup_widget(GTK_WIDGET(button), "checkbutton44");
    check_autre = lookup_widget(GTK_WIDGET(button), "checkbutton45");
    label_status = lookup_widget(GTK_WIDGET(button), "label651");

    // Récupérer les données modifiées
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const char *name_text = gtk_entry_get_text(GTK_ENTRY(entry_name));
    const char *phone_text = gtk_entry_get_text(GTK_ENTRY(entry_phone));
    const char *category_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combobox_category));

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_description));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    char *description_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // 1. Validation des champs obligatoires
    if (strlen(id_text) == 0 || strlen(name_text) == 0 || strlen(phone_text) == 0 ||
        strlen(category_text) == 0 || strlen(description_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Tous les champs doivent être remplis.");
        g_free(description_text);
        return;
    }

    // 2. Vérification du format du nom (commence par une majuscule)
    if (!g_ascii_isalpha(name_text[0]) || !g_ascii_isupper(name_text[0])) {
        char corrected_name[256];
        strcpy(corrected_name, name_text);
        corrected_name[0] = g_ascii_toupper(corrected_name[0]);
        gtk_entry_set_text(GTK_ENTRY(entry_name), corrected_name); // Mettre à jour le champ
        gtk_label_set_text(GTK_LABEL(label_status), "Le champ 'Nom' a été corrigé pour commencer par une majuscule.");
        g_free(description_text);
        return;
    }

    // 3. Vérification que l'ID est un entier positif
    if (!g_ascii_isdigit(id_text[0]) || atoi(id_text) <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : L'ID doit être un entier positif.");
        g_free(description_text);
        return;
    }

    // 4. Vérification du numéro de téléphone (exactement 8 chiffres)
    if (strlen(phone_text) != 8 || strspn(phone_text, "0123456789") != 8) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Le numéro de téléphone doit contenir exactement 8 chiffres.");
        g_free(description_text);
        return;
    }

    // 5. Vérification des valeurs des spin buttons pour l'heure
    int hour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_hour));
    int minute = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_minute));
    int second = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_second));
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : L'heure, les minutes et les secondes doivent être valides.");
        g_free(description_text);
        return;
    }

    // 6. Vérification du prix et de la limite de réservation
    int price = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_price));
    int reservation = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_reservation));
    if (price <= 0 || reservation <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Le prix et la limite de réservation doivent être positifs.");
        g_free(description_text);
        return;
    }

    // 7. Vérification qu'au moins un mode de paiement est sélectionné
    gboolean payment_selected = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_carte)) ||
                                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_espece)) ||
                                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_autre));
    if (!payment_selected) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Au moins un mode de paiement doit être sélectionné.");
        g_free(description_text);
        return;
    }

    // Remplir la structure Service
    service_modifie.id = atoi(id_text);
    strcpy(service_modifie.name, name_text);
    strcpy(service_modifie.phone, phone_text);
    strcpy(service_modifie.category, category_text);
    strcpy(service_modifie.description, description_text);
    g_free(description_text);

    service_modifie.time_hour = hour;
    service_modifie.time_minute = minute;
    service_modifie.time_second = second;
    service_modifie.reservation_limit = reservation;
    service_modifie.price = price;
    service_modifie.remise = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_yes)) ? 1 : 0;
    service_modifie.payment_type[0] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_carte)) ? 1 : 0;
    service_modifie.payment_type[1] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_espece)) ? 1 : 0;
    service_modifie.payment_type[2] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_autre)) ? 1 : 0;

    // Appeler la fonction modifier_service
    if (modifier_service(filename, service_modifie.id, service_modifie)) {
        gtk_label_set_text(GTK_LABEL(label_status), "Service modifié avec succès !");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Service introuvable ou modification échouée.");
    }
}


void
on_button81_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *entry_id, *entry_name, *entry_phone;
    GtkWidget *spin_hour, *spin_minute, *spin_second;
    GtkWidget *spin_reservation, *spin_price;
    GtkWidget *combobox_category, *textview_description;
    GtkWidget *radio_yes, *radio_no;
    GtkWidget *check_carte, *check_espece, *check_autre;

    // Récupération des widgets
    entry_id = lookup_widget(GTK_WIDGET(button), "entry96");
    entry_name = lookup_widget(GTK_WIDGET(button), "entry97");
    entry_phone = lookup_widget(GTK_WIDGET(button), "entry98");
    spin_hour = lookup_widget(GTK_WIDGET(button), "spinbutton71");
    spin_minute = lookup_widget(GTK_WIDGET(button), "spinbutton72");
    spin_second = lookup_widget(GTK_WIDGET(button), "spinbutton73");
    spin_reservation = lookup_widget(GTK_WIDGET(button), "spinbutton74");
    spin_price = lookup_widget(GTK_WIDGET(button), "spinbutton75");
    combobox_category = lookup_widget(GTK_WIDGET(button), "comboboxentry23");
    textview_description = lookup_widget(GTK_WIDGET(button), "textview8");
    radio_yes = lookup_widget(GTK_WIDGET(button), "radiobutton64");
    radio_no = lookup_widget(GTK_WIDGET(button), "radiobutton65");
    check_carte = lookup_widget(GTK_WIDGET(button), "checkbutton43");
    check_espece = lookup_widget(GTK_WIDGET(button), "checkbutton44");
    check_autre = lookup_widget(GTK_WIDGET(button), "checkbutton45");

    // Vider les champs texte
    gtk_entry_set_text(GTK_ENTRY(entry_id), "");
    gtk_entry_set_text(GTK_ENTRY(entry_name), "");
    gtk_entry_set_text(GTK_ENTRY(entry_phone), "");

    // Réinitialiser les spin buttons
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hour), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_minute), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_second), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_reservation), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_price), 0);

    // Réinitialiser la combobox
    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_category), -1);

    // Vider le textview
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_description));
    gtk_text_buffer_set_text(buffer, "", -1);

    // Réinitialiser les boutons radio
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_yes), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_no), FALSE);

    // Réinitialiser les cases à cocher
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_carte), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_espece), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_autre), FALSE);

    // Optionnel : Afficher un message si nécessaire
    GtkWidget *label_status = lookup_widget(GTK_WIDGET(button), "label651");
    if (label_status) {
        gtk_label_set_text(GTK_LABEL(label_status), "Formulaire réinitialisé.");
    }
}



void on_button87_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *entry_id, *combobox_category, *label_status, *treeview9;
    const char *id_text, *category_text;
    int id;
    Service service;
    const char *filename = "parkings.txt";

    // Récupération des widgets
    entry_id = lookup_widget(GTK_WIDGET(button), "entry101");
    combobox_category = lookup_widget(GTK_WIDGET(button), "comboboxentry32");
    label_status = lookup_widget(GTK_WIDGET(button), "label652");
    treeview9 = lookup_widget(GTK_WIDGET(button), "treeview9");

    // Vérification des widgets
    if (!entry_id || !combobox_category || !label_status || !treeview9) {
        g_print("Erreur : Widgets non trouvés.\n");
        return;
    }

    // Configuration des colonnes si nécessaire
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview9));
    if (!model) {
        GtkListStore *store = creer_model_treeview();
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview9), GTK_TREE_MODEL(store));
        configurer_columns_treeview(GTK_TREE_VIEW(treeview9));  // Configure les colonnes
        g_object_unref(store);  // Libère la référence locale
    }

    // Récupération des données
    id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    category_text = NULL;

    // Assurez-vous que c'est un GtkComboBoxText avant d'utiliser la fonction appropriée
    if (GTK_IS_COMBO_BOX(combobox_category)) {
        category_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combobox_category));
    }

    // Réinitialisation du TreeView
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview9)));
    gtk_list_store_clear(store);

    gboolean found = FALSE;

    // Recherche par ID et catégorie combinée
    FILE *file = fopen(filename, "r");
    if (file) {
        char line[512];
        while (fgets(line, sizeof(line), file)) {
            Service temp;
            if (sscanf(line, "%d|%49[^|]|%49[^|]|%199[^|]|%14[^|]|%d|%d|%d|%d|%lf|%d|%d|%d|%d",
                       &temp.id, temp.name, temp.category, temp.description, temp.phone,
                       &temp.time_hour, &temp.time_minute, &temp.time_second,
                       &temp.reservation_limit, &temp.price,
                       &temp.remise, &temp.payment_type[0],
                       &temp.payment_type[1], &temp.payment_type[2]) == 14) {

                // Filtrage par ID si spécifié
                gboolean match = TRUE;
                if (id_text && strlen(id_text) > 0 && temp.id != atoi(id_text)) {
                    match = FALSE;
                }

                // Filtrage par catégorie si spécifiée
                if (category_text && strlen(category_text) > 0 && strcmp(temp.category, category_text) != 0) {
                    match = FALSE;
                }

                // Si une correspondance est trouvée, afficher le service
                if (match) {
                    afficher_service_dans_treeview(temp, GTK_TREE_VIEW(treeview9));
                    found = TRUE;
                }
            }
        }
        fclose(file);
    }

    // Affichage du message de statut
    if (found) {
        gtk_label_set_text(GTK_LABEL(label_status), "Service(s) trouvé(s) et affiché(s).");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "Aucun service trouvé.");
    }
}





void on_button85_clicked(GtkButton *button, gpointer user_data) {
    // Récupération des widgets nécessaires
    GtkWidget *treeview = lookup_widget(GTK_WIDGET(button), "treeview9");
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    GtkTreeModel *model;
    GtkTreeIter iter;

    // Vérifier si une ligne est sélectionnée
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int id;

        // Récupérer l'ID du service à partir de la ligne sélectionnée
        gtk_tree_model_get(model, &iter, 0, &id, -1);

        // Supprimer le service du fichier
        if (supprimer_service("parkings.txt", id)) {
            // Supprimer la ligne du TreeView
            gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

            // Afficher un message de confirmation
            GtkWidget *label_status = lookup_widget(GTK_WIDGET(button), "label652");
            gtk_label_set_text(GTK_LABEL(label_status), "Service supprimé avec succès.");
        } else {
            // Erreur lors de la suppression
            GtkWidget *label_status = lookup_widget(GTK_WIDGET(button), "label652");
            gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Impossible de supprimer le service.");
        }
    } else {
        // Aucun service sélectionné
        GtkWidget *label_status = lookup_widget(GTK_WIDGET(button), "label652");
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Aucun service sélectionné.");
    }
}


void on_button88_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *treeview9, *combobox_status, *label_status;
    const char *status_text;
    gboolean found = FALSE;
    const char *filename = "parkings.txt";

    // Récupération des widgets
    treeview9 = lookup_widget(GTK_WIDGET(button), "treeview9");
    combobox_status = lookup_widget(GTK_WIDGET(button), "comboboxentry32"); // ComboBox où vous sélectionnez l'état
    label_status = lookup_widget(GTK_WIDGET(button), "label652");

    // Vérification des widgets
    if (!treeview9 || !combobox_status || !label_status) {
        g_print("Erreur : Widgets non trouvés.\n");
        return;
    }

    // Récupération de l'état sélectionné dans le ComboBox
    status_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combobox_status));

    // Réinitialisation du TreeView (effacement des données actuelles)
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview9)));
    gtk_list_store_clear(store);

    // Lecture des services dans le fichier et affichage selon l'état
    FILE *file = fopen(filename, "r");
    if (file) {
        char line[512];
        while (fgets(line, sizeof(line), file)) {
            Service temp;
            // Extraction des informations du service depuis la ligne du fichier
            if (sscanf(line, "%d|%49[^|]|%49[^|]|%199[^|]|%14[^|]|%d|%d|%d|%d|%lf|%d|%d|%d|%d",
                       &temp.id, temp.name, temp.category, temp.description, temp.phone,
                       &temp.time_hour, &temp.time_minute, &temp.time_second,
                       &temp.reservation_limit, &temp.price,
                       &temp.remise, &temp.payment_type[0],
                       &temp.payment_type[1], &temp.payment_type[2]) == 14) {

                // Si un état est sélectionné, on filtre selon l'état
                if (status_text && strlen(status_text) > 0) {
                    if (strcmp(temp.category, status_text) == 0) {
                        afficher_service_dans_treeview(temp, GTK_TREE_VIEW(treeview9));
                        found = TRUE;
                    }
                } else {
                    // Si aucun état n'est sélectionné, afficher tous les services
                    afficher_service_dans_treeview(temp, GTK_TREE_VIEW(treeview9));
                    found = TRUE;
                }
            }
        }
        fclose(file);
    }

    // Affichage du message de statut
    if (found) {
        gtk_label_set_text(GTK_LABEL(label_status), "Service(s) trouvé(s) et affiché(s).");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "Aucun service trouvé pour cet état.");
    }
}



void on_button44_activate(GtkButton *button, gpointer user_data) {

}


void
on_button106_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
    // Récupération des widgets
    GtkWidget *entry_id = lookup_widget(GTK_WIDGET(button), "entry125");
    GtkWidget *label_status = lookup_widget(GTK_WIDGET(button), "label655");

    if (!entry_id || !label_status) {
        g_print("Erreur : Widgets non trouvés.\n");
        return;
    }

    // Vérification de l'ID
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    if (!id_text || strlen(id_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Veuillez entrer un ID.");
        g_print("Erreur : L'ID est vide.\n");
        return;
    }

    int id = atoi(id_text);
    if (id <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : L'ID doit être un entier positif.");
        g_print("Erreur : L'ID n'est pas valide (id=%d).\n", id);
        return;
    }

    // Recherche du service
    reserv res = chercher_reserv("reservations.txt", id);
    if (res.id == -1) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : reservation introuvable.");
        g_print("Erreur : reservation introuvable pour l'ID %d.\n", id);
        return;
    }

    // Mise à jour de l'interface avec les informations du service
    char message[256];
    snprintf(message, sizeof(message), "reservation trouvé : %s (%s)", res.nom, res.description);
    gtk_label_set_text(GTK_LABEL(label_status), message);
    g_print("Service trouvé : %s (%s)\n", res.nom, res.description);

}
void
on_button107_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
    // Récupération des widgets
    GtkWidget *entry_id = lookup_widget(GTK_WIDGET(button), "entry126");
    GtkWidget *label_status = lookup_widget(GTK_WIDGET(button), "label656");

    if (!entry_id || !label_status) {
        g_print("Erreur : Widgets non trouvés.\n");
        return;
    }

    // Vérification de l'ID
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    if (!id_text || strlen(id_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Veuillez entrer un ID.");
        g_print("Erreur : L'ID est vide.\n");
        return;
    }

    int id = atoi(id_text);
    if (id <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : L'ID doit être un entier positif.");
        g_print("Erreur : L'ID n'est pas valide (id=%d).\n", id);
        return;
    }

    // Recherche du service
    reserv res = chercher_reserv("parkings.txt", id);
    if (res.id == -1) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Service introuvable.");
        g_print("Erreur : Service introuvable pour l'ID %d.\n", id);
        return;
    }

    // Mise à jour de l'interface avec les informations du service
    char message[256];
    snprintf(message, sizeof(message), "Service trouvé : %s (%s)", res.nom, res.description);
    gtk_label_set_text(GTK_LABEL(label_status), message);
    g_print("Service trouvé : %s (%s)\n", res.nom, res.description);

}

void
on_button108_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *entry_id = lookup_widget(GTK_WIDGET(button), "entry125");   // Entry pour l'ID de réservation
    GtkWidget *entry_id1 = lookup_widget(GTK_WIDGET(button), "entry126");  // Entry pour l'ID du service
    GtkWidget *label_status = lookup_widget(GTK_WIDGET(button), "label657");  // Label pour le statut

    if (!entry_id || !entry_id1 || !label_status) {
        g_print("Erreur : Widgets non trouvés.\n");
        return;
    }

    // Récupération de l'ID de la réservation
    const char *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    if (strlen(id_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Veuillez entrer un ID de réservation.");
        return;
    }

    int reservation_id = atoi(id_text);
    if (reservation_id <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : L'ID de réservation doit être un entier positif.");
        return;
    }

    // Recherche de la réservation
    reserv reservation = chercher_reserv("reservations.txt", reservation_id);
    if (reservation.id == -1) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Réservation introuvable.");
        return;
    }

    // Récupération de l'ID du service
    const char *id_text1 = gtk_entry_get_text(GTK_ENTRY(entry_id1));
    if (strlen(id_text1) == 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Veuillez entrer un ID de service.");
        return;
    }

    int service_id = atoi(id_text1);
    if (service_id <= 0) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : L'ID du service doit être un entier positif.");
        return;
    }

    // Recherche du service correspondant dans parkings.txt
    Service service = chercher_service("parkings.txt", service_id);
    if (service.id == -1) {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Service introuvable.");
        return;
    }

    // Vérification si cette réservation a déjà été affectée à ce service
    FILE *f = fopen("reservations_affectees.txt", "r");
    if (f != NULL) {
        char line[256];
        int res_id, serv_id;
        while (fgets(line, sizeof(line), f)) {
            // Vérification des IDs dans le fichier
            if (sscanf(line, "Réservation ID : %d Service ID : %d", &res_id, &serv_id) == 2) {
                if (res_id == reservation_id && serv_id == service_id) {
                    gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Cette réservation est déjà affectée à ce service.");
                    fclose(f);
                    return;
                }
            }
        }
        fclose(f);
    }

    // Vérification de la limite de réservation du service
    if (service.reservation_limit > 0) {
        // Mise à jour du service en diminuant la limite de réservation
        service.reservation_limit--;  // Diminuer la limite de réservation
        if (modifier_service("parkings.txt", service.id, service)) {
            // Ajouter la réservation affectée au fichier
            f = fopen("reservations_affectees.txt", "a");
            if (f != NULL) {
                fprintf(f, "Réservation ID : %d Service ID : %d\n", reservation.id, service.id);
                fprintf(f, "Nom du client : %s\n", reservation.nom);
                fprintf(f, "Nom du service : %s\n", service.name);
                fprintf(f, "Description du service : %s\n", service.description);
                fprintf(f, "Limite de réservation restante : %d\n", service.reservation_limit);
                fprintf(f, "Prix du service : %.2f\n", service.price);
                fprintf(f, "----------------------------------------\n");
                fclose(f);

                gtk_label_set_text(GTK_LABEL(label_status), "Réservation affectée avec succès !");
            } else {
                gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Impossible de mettre à jour le fichier.");
            }
        } else {
            gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Échec de la mise à jour de la limite de réservation.");
        }
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "Erreur : Plus de places disponibles.");
    }
}



void on_button109_clicked(GtkButton *button, gpointer user_data) {
GtkWidget *treeview9, *combobox_status, *label_status;
    const char *status_text;
    gboolean found = FALSE;
    const char *filename = "parkings.txt";

    // Récupération des widgets
    treeview9 = lookup_widget(GTK_WIDGET(button), "treeview13");
    combobox_status = lookup_widget(GTK_WIDGET(button), "combo56"); // ComboBox où vous sélectionnez l'état
    label_status = lookup_widget(GTK_WIDGET(button), "label658");

    // Vérification des widgets
    if (!treeview9 || !combobox_status || !label_status) {
        g_print("Erreur : Widgets non trouvés.\n");
        return;
    }

    // Récupération de l'état sélectionné dans le ComboBox
    status_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combobox_status));

    // Réinitialisation du TreeView (effacement des données actuelles)
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview9)));
    gtk_list_store_clear(store);

    // Lecture des services dans le fichier et affichage selon l'état
    FILE *file = fopen(filename, "r");
    if (file) {
        char line[512];
        while (fgets(line, sizeof(line), file)) {
            Service temp;
            // Extraction des informations du service depuis la ligne du fichier
            if (sscanf(line, "%d|%49[^|]|%49[^|]|%199[^|]|%14[^|]|%d|%d|%d|%d|%lf|%d|%d|%d|%d",
                       &temp.id, temp.name, temp.category, temp.description, temp.phone,
                       &temp.time_hour, &temp.time_minute, &temp.time_second,
                       &temp.reservation_limit, &temp.price,
                       &temp.remise, &temp.payment_type[0],
                       &temp.payment_type[1], &temp.payment_type[2]) == 14) {

                // Si un état est sélectionné, on filtre selon l'état
                if (status_text && strlen(status_text) > 0) {
                    if (strcmp(temp.category, status_text) == 0) {
                        afficher_service_dans_treeview(temp, GTK_TREE_VIEW(treeview9));
                        found = TRUE;
                    }
                } else {
                    // Si aucun état n'est sélectionné, afficher tous les services
                    afficher_service_dans_treeview(temp, GTK_TREE_VIEW(treeview9));
                    found = TRUE;
                }
            }
        }
        fclose(file);
    }

    // Affichage du message de statut
    if (found) {
        gtk_label_set_text(GTK_LABEL(label_status), "Service(s) trouvé(s) et affiché(s).");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "Aucun service trouvé pour cet état.");
    }
}



void
on_button110_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button141_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{


    GtkWidget *entry_id, *combobox_category, *label_status, *treeview9;
    const char *id_text, *category_text;
    int id;
    Service service;
    const char *filename = "parkings.txt";

    // Récupération des widgets
    entry_id = lookup_widget(GTK_WIDGET(button), "entry126");
    combobox_category = lookup_widget(GTK_WIDGET(button), "combo56");
    label_status = lookup_widget(GTK_WIDGET(button), "label658");
    treeview9 = lookup_widget(GTK_WIDGET(button), "treeview13");

    // Vérification des widgets
    if (!entry_id || !combobox_category || !label_status || !treeview9) {
        g_print("Erreur : Widgets non trouvés.\n");
        return;
    }

    // Configuration des colonnes si nécessaire
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview9));
    if (!model) {
        GtkListStore *store = creer_model_treeview();
        gtk_tree_view_set_model(GTK_TREE_VIEW(treeview9), GTK_TREE_MODEL(store));
        configurer_columns_treeview(GTK_TREE_VIEW(treeview9));  // Configure les colonnes
        g_object_unref(store);  // Libère la référence locale
    }

    // Récupération des données
    id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    category_text = NULL;

    // Assurez-vous que c'est un GtkComboBoxText avant d'utiliser la fonction appropriée
    if (GTK_IS_COMBO_BOX(combobox_category)) {
        category_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combobox_category));
    }

    // Réinitialisation du TreeView
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview9)));
    gtk_list_store_clear(store);

    gboolean found = FALSE;

    // Recherche par ID et catégorie combinée
    FILE *file = fopen(filename, "r");
    if (file) {
        char line[512];
        while (fgets(line, sizeof(line), file)) {
            Service temp;
            if (sscanf(line, "%d|%49[^|]|%49[^|]|%199[^|]|%14[^|]|%d|%d|%d|%d|%lf|%d|%d|%d|%d",
                       &temp.id, temp.name, temp.category, temp.description, temp.phone,
                       &temp.time_hour, &temp.time_minute, &temp.time_second,
                       &temp.reservation_limit, &temp.price,
                       &temp.remise, &temp.payment_type[0],
                       &temp.payment_type[1], &temp.payment_type[2]) == 14) {

                // Filtrage par ID si spécifié
                gboolean match = TRUE;
                if (id_text && strlen(id_text) > 0 && temp.id != atoi(id_text)) {
                    match = FALSE;
                }

                // Filtrage par catégorie si spécifiée
                if (category_text && strlen(category_text) > 0 && strcmp(temp.category, category_text) != 0) {
                    match = FALSE;
                }

                // Si une correspondance est trouvée, afficher le service
                if (match) {
                    afficher_service_dans_treeview(temp, GTK_TREE_VIEW(treeview9));
                    found = TRUE;
                }
            }
        }
        fclose(file);
    }

    // Affichage du message de statut
    if (found) {
        gtk_label_set_text(GTK_LABEL(label_status), "Service(s) trouvé(s) et affiché(s).");
    } else {
        gtk_label_set_text(GTK_LABEL(label_status), "Aucun service trouvé.");
    }
}



