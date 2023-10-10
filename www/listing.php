<?php
    ob_start();

    $directory = isset($_GET['dir']) ? $_GET['dir'] : '.';

    //UNSAFE NEED CHECKS

    if (is_dir($directory)) {
        $files = scandir($directory);
        echo "<ul>";
        foreach ($files as $file) {
            if ($file != "." && $file != "..") {
                echo "<li><a href='{$directory}/{$file}'>{$file}</a></li>";
            }
        }
        echo "</ul>";
    } else {
        echo "{$directory} is not a valid directory";
    }

    $output = ob_get_clean();
    $content_length = strlen($output);
    header("Content-Length: $content_length");
    echo $output;
?>