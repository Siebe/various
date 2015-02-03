<?php

require __DIR__.'/lib/rb/rb.php';
require __DIR__.'/lib/AltoRouter/AltoRouter.php';
require __DIR__.'/lib/ObjectPeer.php';
require __DIR__.'/lib/ImageHelper.php';
require __DIR__.'/lib/Config.php';

require __DIR__.'/action/FrontendActions.php';

setlocale(LC_TIME, array('Dutch_Netherlands', 'Dutch', 'nl_NL', 'nl', 'nl_NL.ISO8859-1', 'nld_NLD'));
//ini_set('display_errors', '0'); 

R::setup(Config::$db, Config::$db_user, Config::$db_pass);

$router = new AltoRouter();

$router->addRoutes(array(
  array('GET',  '/',                        'FrontendActions#mainAction',       'main' ),
  array('GET',  '/werk/[:reference]?',      'FrontendActions#workAction',       'work'),
  array('GET',  '/details/[:reference]',    'FrontendActions#detailsAction',    'details'),
  array('GET',  '/nieuws/[:reference]?',    'FrontendActions#postAction',       'post'), 
  array('GET',  '/contact',                 'FrontendActions#contactAction',    'contact'), 
  array('GET',  '/over',                    'FrontendActions#aboutAction',      'about'),
    
  array('GET',  '/afbeelding/[:reference]/[:details_id]?', 'FrontendActions#imageAction',      'image'),  

  array('GET',  '/email', 'FrontendActions#emailAction',      'email'),
    
  array('GET|POST',  '/backend',                                'BackendActions#mainAction',    'back_main'),  
  array('GET',       '/backend/loguit',                         'BackendActions#logoutAction',  'back_logout'),  
  array('POST|GET',  '/backend/werk/[a:act]?/[:reference]?',    'BackendActions#workAction',    'back_work'), 
  array('POST|GET',  '/backend/nieuws/[a:act]?/[:reference]?',  'BackendActions#postAction',    'back_post'), 
  array('POST|GET',  '/backend/details/[a:act]?/[:reference]?', 'BackendActions#detailsAction', 'back_details'), 
  array('POST|GET',  '/backend/contact/[a:act]?/[:reference]?', 'BackendActions#contactAction', 'back_contact'),
  array('POST|GET',  '/backend/over/[a:act]?/[:reference]?',    'BackendActions#aboutAction',   'back_about'),
    
  array('GET','/empty','FrontendActions#emptyAction','empty'),  
  array('GET','/[:reference]','FrontendActions#autoAction','auto'),
    
));




$match = $router->match();
$backend = false;
// call closure or throw 404 status
if ($match) {
    $backend = (substr($match['name'], 0, 5) == "back_");
    //load additional code for backend here
    if ($backend) {
        require __DIR__.'/lib/Auth.php';
        require __DIR__.'/lib/FormHelper.php';
        require __DIR__.'/lib/DetailsHelper.php';
        require __DIR__.'/action/BackendActions.php';
    }
    
    list( $controller_class, $action ) = explode( '#', $match['target'] );

    if (is_callable(array($controller_class, $action)) ) {
        $controller = new $controller_class();
        call_user_func_array(array($controller,$action), array($match['params']));
    } else {
        //404
        $view = "500";
        $error = "Router wijst verkeerd.";
    }

} else {
	// no route was matched
	$view = "404";
}


//Extract controller parameters and load a view
if (isset($controller)) {
    extract((array)$controller);
}
if (!isset($view)) {
    $view = $match['name'];
}
if ($view != 'none') {
    ob_start();
    require("./view/".$view.".php");
    $strView = ob_get_contents();
    ob_end_clean();
    echo $strView;
}

?>

