<!DOCTYPE HTML>

<html>
    <head>
    <META NAME="ROBOTS" CONTENT="NOINDEX, NOFOLLOW">
    <title>Domeijer Collages - Backend</title>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <?php 
    if (isset($scripts)) {
        foreach ($scripts as $script) {
            echo '
    <script src="'.$script.'" type="text/javascript"></script>'; 
        }
    }
    ?>
    <?php 
    if (isset($stylesheets)) {
        foreach ($stylesheets as $sheet) {
            echo '
    <link rel="stylesheet" href="'.$sheet.'" type="text/css">'; 
        }
    }
    ?>
 <!--  <script src="/view/js/scalebackground.js" type="text/javascript"></script> -->
    <link  href="/view/css/main.css" rel="stylesheet" type="text/css">
    <link  href="/view/css/backend.css" rel="stylesheet" type="text/css">
    <link href="/view/img/favicon.png" rel="icon" type="image/png">
</head>
<body>

    <div class="main" id="main">
        <div class="header">
            <h2>Backend</h2>
        </div>
        <div class ="menu">
            <a href="/backend/werk">
                <div class="menu-button button">
                    Werk
                </div>
            </a>
            <a href="/backend/over">
                <div class="menu-button button">
                    Over
                </div>
            </a>
            <a href="/backend/nieuws">
                <div class="menu-button button">
                    Nieuws
                </div>
            </a>
            <a href="/backend/contact">
                <div class="menu-button button">
                    Contact
                </div>
            </a>
            <div class="menu-spacer"></div>
        </div>
        
        <div class ="content" id="content">
            
            
            