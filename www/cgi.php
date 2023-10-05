<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $f = fopen("php://stdin", "rb");

    echo "Hey\n";
    while($line = fgets($f))
    {
    echo $line;
    }

    fclose($f);

}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>File Upload</title>
</head>
<body>
    <form action="" method="POST" enctype="multipart/form-data">
        <!-- File input to select multiple files -->
        <input type="file" name="file_upload[]" multiple>
        <button type="submit">Upload</button>
    </form>
</body>
</html>
