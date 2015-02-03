<!DOCTYPE HTML>

<html>
    <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>Domeijer Collages<?php echo isset($title_append) ?  ' - '.$title_append : ''?></title> 
<?php 
    if (isset(Config::$description)) {
        $description = Config::$description.(isset($description_append)? $description_append : '');
        echo '    <meta name="description" content="'.$description.'">'."\n";
    }
    if (isset(Config::$keywords)) {
        echo '    <meta name="keywords" content="'.Config::$keywords.'">'."\n";
    }
    if (isset($scripts)) {
        foreach ($scripts as $script) {
            echo '    <script src="'.$script.'" type="text/javascript"></script>'."\n"; 
        }
    }
    if (isset($stylesheets)) {
        foreach ($stylesheets as $sheet) {
            echo '    <link rel="stylesheet" href="'.$sheet.'" type="text/css">'."\n"; 
        }
    }
    ?>
    <script src="/view/js/google.analytics.js" type="text/javascript"></script>
    <script type="text/javascript" src="//s7.addthis.com/js/300/addthis_widget.js#pubid=ra-54b56cd666a996f9" async="async"></script>

    <link  href="/view/css/main.css" rel="stylesheet" type="text/css">
    <noscript>
        <link  href="/view/css/noscript.css" rel="stylesheet" type="text/css">
    </noscript>
    <link href="/view/img/favicon.png" rel="icon" type="image/png">
    <meta name="google-site-verification" content="8SCggmteewpJe3Al1-iACUwitWLqed8fu-bH4_WKRpw" />
</head>

<body>
    <h1><?php echo $description ?></h1>
    
    <div class="background">
        <div id="bg-left" class="bg-half">
            <img src="/view/img/bg-flower.png" alt="background" />
        </div>
        <div id="bg-right" class="bg-half">
            <img src="/view/img/bg-flower.png" alt="background" />
        </div>
    </div>
    
    <div class="main" id="main">
        
        <div class="header">
            <h2>Domeijer Collages</h2>
        </div>
        <div class ="menu">
            <a href="/">
                <div class="menu-button button">
                    Werk
                </div>
            </a>
            <a href="/over">
                <div class="menu-button button">
                    Over
                </div>
            </a>
            <a href="/nieuws">
                <div class="menu-button button">
                    Nieuws
                </div>
            </a>
            <a href="/contact">
                <div class="menu-button button">
                    Contact
                </div>
            </a>
            <div class="menu-spacer"></div>
        </div>
        
        <div class ="content" id="content">
            
            
            