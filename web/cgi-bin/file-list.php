<?php
$files = scandir('../uploads');
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Archive des fichiers</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            color: #333;
        }
        .container {
            width: 80%;
            margin: 0 auto;
        }
        h1 {
            text-align: center;
            color: #555;
        }
        ul {
            list-style-type: none;
            padding: 0;
        }
        li {
            background: #fff;
            margin: 10px 0;
            padding: 15px;
            border: 1px solid #ddd;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        .actions {
            display: flex;
            gap: 10px;
        }
        .btn {
            padding: 8px 12px;
            border: none;
            color: #fff;
            cursor: pointer;
            text-decoration: none;
            border-radius: 5px;
        }
        .btn-download {
            background-color: #4CAF50;
        }
        .btn-delete {
            background-color: #f44336;
        }
        .btn:hover {
            opacity: 0.9;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Archive des fichiers</h1>
        <ul>
            <?php foreach ($files as $file): ?>
                <?php if ($file !== '.' && $file !== '..'): ?>
                    <li>
                        <span><?php echo htmlspecialchars($file); ?></span>
                        <div class="actions">
                            <a href="download.php?file=<?php echo urlencode($file); ?>" class="btn btn-download" download onclick="confirmAndDownload('<?php echo urlencode($file); ?>');">Download</a>
                            <a href="delete.php?file=<?php echo urlencode($file); ?>" class="btn btn-delete" onclick="confirmAndDelete('<?php echo urlencode($file); ?>'); return false;">Delete</a>
                        </div>
                    </li>
                <?php endif; ?>
            <?php endforeach; ?>
        </ul>
        <button onclick="window.location.href='http://localhost/';">Go back</button>   
    </div>
    <script>
        function confirmAndDownload(filename) {
            fetch(`download.php?file=${filename}`, {
                method: 'GET',
            })
        }
        function confirmAndDelete(filename) {
            fetch(`delete.php?file=${filename}`, {
                method: 'DELETE',
            })
            .then(response => {
                if (response.ok) {
                    location.reload();
                }
            })
        }
    </script>
</body>
</html>