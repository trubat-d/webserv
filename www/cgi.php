<?php
ob_start();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>File Upload</title>
<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST')
{


    echo "SALUT\n";
    $files = $_FILES['userfile'];
    $count_files = count($_FILES['userfile']['name']);
    $upload_dir = "./upload/";
    $valid_image = array("png", "jpg", "jpeg");
    for($i = 0; $i < $count_files; $i++)
    {
        $temp_name = $files['tmp_name'][$i];
        $end_name = $files['name'][$i];
        echo "<p>Uploading file : $end_name </p>";
        move_uploaded_file($files['tmp_name'][$i], "$upload_dir$end_name");
        echo "<p>File $end_name saved</p>";
        if(in_array(pathinfo($_FILES['userfile']['name'][$i])['extension'], $valid_image))
        {
            echo "<img src='$upload_dir$end_name'>";
        }
    }
    echo "\nFIN\n";
//     print_r(array_keys($_FILES["userfile"]["name"]));
//     print_r(array_keys($_POST));


}
?>
</head>
<body>
</body>
</html>
<?php
    $output = ob_get_clean();
    $content_length = strlen($output);
    header("Content-Length: $content_length");
    echo $output;
?>