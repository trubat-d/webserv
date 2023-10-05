<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST')
{


    echo "SALUT\n";
    $data = $_FILES["userfile"]["name"];
    echo "\nFIN\n";


}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>File Upload</title>
    <>
</head>
<body>
    <h1>File uploaded</h1>
</body>
</html>
