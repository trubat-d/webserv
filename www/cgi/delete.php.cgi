<?php
    ob_start();
    if ($_SERVER['REQUEST_METHOD'] === 'DELETE') {
        $filename = $_SERVER['HTTP_X_FILENAME'];
        echo $filename;
        if ($filename) {
            $filePath = "../upload/" . $filename;
            if (file_exists($filePath)) {
                unlink($filePath);
                echo json_encode(["message" => "File deleted successfully"]);
                header("Status: 200 OK");
            } else {
                header("Status: 404 Not Found");
                echo json_encode(["message" => "File not found"]);
            }
        } else {
            header("Status: 400 Bad Request");
            echo json_encode(["message" => "Filename is not set"]);
        }
    } else {
        header("Status: 405 Method Not Allowed");
        echo json_encode(["message" => "Invalid request method"]);
    }
    $output = ob_get_clean();
    $content_length = strlen($output);
    header("Content-Length: $content_length");
    echo $output;
?>

