<?php
// Configuración de la base de datos
$host = "localhost";
$user = "root";
$pass = "";
$db = "carromt";

// Conexión a la base de datos
$conn = new mysqli($host, $user, $pass, $db);
if ($conn->connect_error) {
    die("Conexión fallida: " . $conn->connect_error);
}

// Enviar comando al Arduino si existe un comando POST
if (isset($_POST['command'])) {
    $command = $_POST['command'];  // Captura el comando enviado desde el botón
    $ip = "192.168.0.177";         // Dirección IP del Arduino (ajústala según corresponda)
    
    // Realiza una solicitud HTTP con el comando enviado
    $url = "http://$ip/?data=$command";
    file_get_contents($url);  // Envía el comando al servidor Arduino
    
    //echo "<p>Comando enviado al Arduino: <strong>" . htmlspecialchars($command) . "</strong></p>";
}

// Consulta SQL para recuperar temperaturas
$sql = "SELECT id, temperatura, fechaHora FROM temps ORDER BY fechaHora DESC";
$result = $conn->query($sql);

?>

<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="refresh" content="10">
	<title>Control Remoto Arduino y Temperaturas</title>
    <style>
        body { 
            text-align: center; 
            font-family: Arial, sans-serif; 
            margin-top: 30px; 
            background-color: #f4f4f9; 
        }
        h1 { 
            margin-bottom: 20px; 
            color: #333; 
        }
        button {
            width: 120px; height: 50px; margin: 10px; font-size: 16px;
            background-color: #4CAF50; color: white; border: none; cursor: pointer;
        }
        button:hover { background-color: #45a049; }
        
        table {
            margin: 20px auto;
            border-collapse: collapse;
            width: 70%;
            box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
            background-color: #fff;
        }
        th, td {
            border: 1px solid #ddd;
            padding: 10px;
            text-align: center;
        }
        th {
            background-color: #4CAF50;
            color: white;
        }
        tr:nth-child(even) {
            background-color: #f2f2f2;
        }
        tr:hover {
            background-color: #ddd;
        }
    </style>
</head>
<body>
    <h1>Control Remoto Arduino</h1>
    <form method="post">
        <!-- Botones para enviar comandos -->
        <button type="submit" name="command" value="F">Adelante</button><br>
        <button type="submit" name="command" value="L">Izquierda</button>
        <button type="submit" name="command" value="S">Parar</button>
        <button type="submit" name="command" value="R">Derecha</button><br>
        <button type="submit" name="command" value="B">Atrás</button>
    </form>

    <h1>Historial de Temperaturas</h1>
    <table>
        <tr>
            <th>ID</th>
            <th>Temperatura (°C)</th>
            <th>Fecha y Hora</th>
        </tr>
        <?php
        if ($result->num_rows > 0) {
            while ($row = $result->fetch_assoc()) {
                echo "<tr>";
                echo "<td>" . $row['id'] . "</td>";
                echo "<td>" . $row['temperatura'] . "</td>";
                echo "<td>" . $row['fechaHora'] . "</td>";
                echo "</tr>";
            }
        } else {
            echo "<tr><td colspan='3'>No hay datos disponibles</td></tr>";
        }
        $conn->close();
        ?>
    </table>
</body>
</html>
