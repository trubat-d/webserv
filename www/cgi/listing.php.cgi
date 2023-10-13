<?php
ob_start();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>File Management</title>
</head>
<body>
    <h1>Manage Files</h1>
    <ul>
        <?php
        $dir = "../upload/";
        $files = scandir($dir);

        foreach ($files as $file) {
            if ($file === "." || $file === "..") continue;
            echo "<li>";
            echo $file;
            echo "<button onclick=\"deleteFile('";
            echo $file;
            echo "')\">Delete</button> </li>";
        }
        ?>
    </ul>

    <script>
        function deleteFile(filename) {
            fetch('delete.php.cgi', {
                method: 'DELETE',
                headers: {
                    'Content-Type': 'application/json',
                    'X-Filename': filename,
                },
            })
            .then(data => {
            window.location.reload();
            })
        }
    </script>

    <br><h2><a href="/">Go to Home</a></h2>
</body>
</html>
<?php
    $output = ob_get_clean();
    $content_length = strlen($output);
    header("Status: 200 OK");
    header("Content-Length: $content_length");
    echo $output;
?>