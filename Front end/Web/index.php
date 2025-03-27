<?php
// signup.php

// Paramètres de connexion à la base de données
$servername = "localhost";
$dbUsername = "root";       // À adapter selon votre configuration
$dbPassword = "";           // À adapter selon votre configuration
$dbName = "test";           // Nom de votre base de données

// Création de la connexion
$conn = new mysqli($servername, $dbUsername, $dbPassword, $dbName);

// Vérification de la connexion
if ($conn->connect_error) {
  die("La connexion a échoué : " . $conn->connect_error);
}

// Vérifier que le formulaire a été soumis
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
  $username = $conn->real_escape_string($_POST['username']);
  $email = $conn->real_escape_string($_POST['email']);
  // Hachage sécurisé du mot de passe
  $password = password_hash($_POST['password'], PASSWORD_DEFAULT);

  // Insertion dans la table 'users'
  $sql = "INSERT INTO users (username, email, password) VALUES ('$username', '$email', '$password')";

  if ($conn->query($sql) === TRUE) {
    echo "Compte créé avec succès ! <a href='login.html'>Cliquez ici pour vous connecter</a>.";
  } else {
    echo "Erreur : " . $sql . "<br>" . $conn->error;
  }
}

$conn->close();
?>
<?php
// login.php
session_start();

// Paramètres de connexion à la base de données
$servername = "localhost";
$dbUsername = "root";       // À adapter selon votre configuration
$dbPassword = "";           // À adapter selon votre configuration
$dbName = "test";           // Nom de votre base de données

// Création de la connexion
$conn = new mysqli($servername, $dbUsername, $dbPassword, $dbName);

// Vérification de la connexion
if ($conn->connect_error) {
  die("La connexion a échoué : " . $conn->connect_error);
}

// Vérifier que le formulaire a été soumis
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
  $username = $conn->real_escape_string($_POST['username']);
  $password = $_POST['password'];

  // Recherche de l'utilisateur dans la base de données
  $sql = "SELECT * FROM users WHERE username='$username'";
  $result = $conn->query($sql);

  if ($result && $result->num_rows > 0) {
    $user = $result->fetch_assoc();
    // Vérification du mot de passe
    if (password_verify($password, $user['password'])) {
      // Authentification réussie, création de la session
      $_SESSION['user_id'] = $user['id'];
      $_SESSION['username'] = $user['username'];
      echo "Connexion réussie ! Bienvenue, " . htmlspecialchars($user['username']) . ".";
      // Vous pouvez rediriger vers une page protégée avec header("Location: dashboard.php");
    } else {
      echo "Mot de passe incorrect.";
    }
  } else {
    echo "Nom d'utilisateur introuvable.";
  }
}

$conn->close();
?>
