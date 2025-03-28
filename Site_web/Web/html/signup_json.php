<?php
// signup_json.php

// Emplacement du fichier JSON
$jsonFile = __DIR__ . '/users.json';

// Vérifier si le formulaire a été soumis
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = $_POST['username'] ?? '';
    $email    = $_POST['email'] ?? '';
    $password = $_POST['password'] ?? '';

    // Lire le fichier JSON existant
    if (!file_exists($jsonFile)) {
        $users = []; // Fichier non existant => tableau vide
    } else {
        $jsonData = file_get_contents($jsonFile);
        $users = json_decode($jsonData, true);
        if (!is_array($users)) {
            $users = [];
        }
    }

    // Vérifier si le username ou l'email existe déjà
    foreach ($users as $user) {
        if ($user['username'] === $username) {
            exit("Erreur : Nom d'utilisateur déjà pris.");
        }
        if ($user['email'] === $email) {
            exit("Erreur : Email déjà utilisé.");
        }
    }

    // Hacher le mot de passe
    $hashedPassword = password_hash($password, PASSWORD_DEFAULT);

    // Créer un nouvel utilisateur
    $newUser = [
        'username' => $username,
        'email'    => $email,
        'password' => $hashedPassword
    ];

    // Ajouter au tableau puis réécrire le JSON
    $users[] = $newUser;
    file_put_contents($jsonFile, json_encode($users, JSON_PRETTY_PRINT));

    echo "Inscription réussie ! <a href='login_json.html'>Connectez-vous ici</a>";
    exit();
}
?>

