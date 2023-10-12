<?php
ob_start();
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>File Upload</title>
</head>
<body>
<h2><a href="/">Go to Home</a></h2>
</body>
<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST')
{
    $files = $_FILES['userfile'];
    $count_files = count($_FILES['userfile']['name']);
    $upload_dir = "../upload/";
    $valid_image = array("png", "jpg", "jpeg");
    for($i = 0; $i < $count_files; $i++)
    {
        $temp_name = $files['tmp_name'][$i];
        $end_name = $files['name'][$i];
        echo "<p>Uploading file : $end_name </p>";
        $result = move_uploaded_file($files['tmp_name'][$i], "$upload_dir$end_name");
        if($result)
            echo "<p>File $end_name saved</p>";
        else
        {
            echo "<p>Error Saving the file</p>";
            header("HTTP/1.1 500 Internal Server Error");
        }
        if(in_array(pathinfo($_FILES['userfile']['name'][$i])['extension'], $valid_image))
        {
            echo "<img src='$upload_dir$end_name'>";
        }
    }


}
?>
</html>
<?php
    $output = ob_get_clean();
    $content_length = strlen($output);
    header("Content-Length: $content_length");
    echo $output;
?>