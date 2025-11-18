#  Gestion des Étudiants - Système de Gestion Académique

Un système complet de gestion des étudiants et des matières développé en langage C, permettant la saisie, modification, suppression et consultation des données académiques.

## 📋 Description

Ce projet est une application en mode console développée en C qui permet de gérer :
- **Les étudiants** (matricule, nom, prénom, date/lieu de naissance, adresse)
- **Les matières** (code, désignation, coefficient)
- **Les notes** des étudiants par matière
- **Les calculs de moyennes** et différents tris des données

## ✨ Fonctionnalités

### 👨‍🎓 Gestion des Étudiants
- ✅ Ajout, modification et suppression d'étudiants
- ✅ Affichage dans différents ordres :
  - Ordre d'enregistrement
  - Ordre alphabétique (ascendant/descendant)
  - Ordre par moyenne (croissant/décroissant)
- ✅ Vérification des matricules uniques

### 📚 Gestion des Matières
- ✅ Ajout, modification et suppression de modules
- ✅ Affichage dans différents ordres :
  - Ordre d'enregistrement
  - Ordre alphabétique (ascendant/descendant)
  - Ordre par moyenne (croissant/décroissant)

### 📊 Gestion des Notes
- ✅ Saisie des notes pour chaque étudiant dans chaque matière
- ✅ Consultation des notes par étudiant
- ✅ Calcul automatique des moyennes générales
- ✅ Calcul des moyennes par matière

### 💾 Persistance des Données
- ✅ Sauvegarde automatique dans des fichiers texte :
  - `etudiants.txt`
  - `modules.txt`
  - `notes.txt`

## 🛠️ Technologies Utilisées

- **Langage** : C
- **Structures de données** : Tableaux statiques, structures
- **Gestion fichiers** : Fonctions standard C (stdio.h)
- **Interface** : Mode console avec menus interactifs
- **Compatibilité** : Windows (avec MessageBox) et autres systèmes


## 🚀 Installation et Compilation

### Prérequis
- Compilateur C (GCC recommandé)
- Système d'exploitation : Windows, Linux ou macOS

### Compilation
```bash
gcc -o gestion_etudiants miniprojet.c
./gestion_etudiants
