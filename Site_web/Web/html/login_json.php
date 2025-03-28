<?php
session_start();

$jsonFile = __DIR__ . '/users.json';

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = $_POST['username'] ?? '';
    $password = $_POST['password'] ?? '';

    if (!file_exists($jsonFile)) {
        exit("Aucun utilisateur enregistré. <a href='signup_json.html'>Inscrivez-vous</a>");
    }

    $jsonData = file_get_contents($jsonFile);
    $users = json_decode($jsonData, true);
    if (!is_array($users)) {
        exit("Erreur : Fichier JSON invalide.");
    }

    // Rechercher l'utilisateur
    foreach ($users as $user) {
        if ($user['username'] === $username) {
            if (password_verify($password, $user['password'])) {
                $_SESSION['username'] = $username;
                echo "Connexion réussie ! Bienvenue, " . htmlspecialchars($username) . ".";
                // header("Location: dashboard.php"); // si tu veux rediriger
                exit();
            } else {
                exit("Mot de passe incorrect.");
            }
        }
    }

    exit("Nom d'utilisateur introuvable.");
}
?>
