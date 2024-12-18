<?php
// Configuración de la base de datos
$host = "localhost";
$user = "root";  // Usuario por defecto en XAMPP
$pass = "";      // Contraseña (por defecto en XAMPP es vacía)
$db = "carromt";

// Conexión a la base de datos
$conn = new mysqli($host, $user, $pass, $db);

// Verifica la conexión
if ($conn->connect_error) {
    die("Conexión fallida: " . $conn->connect_error);
}

// Obtiene el parámetro 'temp' de la URL
if (isset($_GET['temp'])) {
    $temperatura = floatval($_GET['temp']);

    // Inserta la temperatura en la base de datos
    $sql = "INSERT INTO temps (temperatura) VALUES ($temperatura)";

    if ($conn->query($sql) === TRUE) {
        echo "Temperatura guardada correctamente";
    } else {
        echo "Error: " . $sql . "<br>" . $conn->error;
    }
} else {
    echo "No se recibió ninguna temperatura";
}

$conn->close();
?>
