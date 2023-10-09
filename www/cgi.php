<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST')
{


    echo "SALUT\n";
    $data = $_FILES["userfile"]["name"];
//     $data2 = $_POST["userfile"];
//     move_uploaded_file($data, "./test");
    echo $data;
    echo "\nFIN\n";
//     print_r(array_keys($_FILES));
//     print_r(array_keys($_POST));
    echo "\nFIN2\n";


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
</body>
</html>
