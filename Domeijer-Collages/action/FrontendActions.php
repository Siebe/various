<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of FrontendActions
 *
 * @author User
 */
class FrontendActions {
    
    public $view;
    public $all_work;
    public $all_post;
    public $content;
    public $start_index;
    
    public function __construct() {
        $this->application = 'frontend';
    }

    public function mainAction() {
        $this->view = 'work';
        $this->workAction(null);

    }

    public function workAction($param) {
        $work = OP::getObjectByIdOrTitle('work', $param);
        $this->scripts[] = '/view/js/slider/jssor.slider.min.js';
        $this->scripts[] = '/view/js/slider/scaleslider.js';
        $this->stylesheets[] = '/view/css/slider.css';

        $this->start_index = $work ? $work->id : 0;
        $this->all_work = R::findAll('work', "WHERE published = 1 ORDER BY updated_at DESC"); 

        return;
    }


    public function postAction($param) {
        $post = OP::getObjectByIdOrTitle('post', $param);
        //$this->scripts[] = 'view/js/slider/js/jssor.slider.min.js';
        //$this->stylesheets[] = 'view/css/slider.css';
        $this->title_append = "Nieuws";
        $this->start_index = $post ? $post->id : 0;
        $this->all_post = R::findAll('post',"WHERE published = 1 ORDER BY created_at DESC"); 

        return;   
    }


    public function contactAction($param) {
        $this->title_append = "Contact";
        $this->loadContent('contact');
    }
    
    
    public function aboutAction($param) {
        $this->title_append = "Over";
        $this->loadContent('about');
    }      


    public function imageAction($param) {
        $this->view = "none";
        $this->work = OP::getObjectByIdOrTitle('work', $param, false);
        if ($this->work ) { 
            $file = getcwd().$this->work->image;
            $details_id = '';
            if (isset($param['details_id'])) {
                $details = R::load('details', $param['details_id']);
                if ($details && $details->work_id = $this->work->id) {
                    $file = getcwd().$details->image;
                }
                $details_id = '-detail'.$details->id;
            }
            if (!file_exists($file) || !getimagesize($file)) {
                $this->view = 500;
                $this->error = "Bestand niet gevonden: ".$file;
                return;
            }
            $array = explode('.', $file);
            $ext = array_pop($array);
            $filename_out = "Domeijer-".ucfirst($this->work->url_title).$details_id.".".$ext;
            header('Content-Type: image/jpeg');
            header('Content-Disposition: inline; filename="'.$filename_out.'"');
            header('Content-Length: ' . filesize($file));
            readfile($file);
        } else {
            $this->view = 500;
            $this->error = "Werk niet gevonden!";
        }
    }
    
    public function detailsAction($param) {
        $this->work = OP::getObjectByIdOrTitle('work', $param, false);
        if (!$this->work ) { 
            $this->view = 500;
            $this->error = "Werk niet gevonden!";
            return;
        }
        
        $this->title_append = $this->work->title;
        $this->description_append = ' '.$this->work->title.' - '.preg_replace('/(\n|\r)/', ' ', $this->work->description);
        $all_url = R::getCol("SELECT url_title FROM work WHERE published = 1 ORDER BY updated_at DESC");
        $size = sizeof($all_url);
        $index = array_search($this->work->url_title, $all_url);
        $next = ($index < 1) ? $size-1 : ($index - 1) % $size;
        $prev = ($index + 1) % $size;
        $this->next_url = '/'.$all_url[$next];
        $this->previous_url = '/'.$all_url[$prev];
        
        $this->all_details = $this->work->xownDetailsList;
        
    }
    
    public function emailAction($param) {
        $this->view = 'empty';
        header("Location: mailto:domeijer@casema.nl");
        $this->message = '<h3>Email word geladen</h3>';
    }
    
    
    
    public function autoAction($param) {
        if (isset($param['reference'])) {
            $ref = $param['reference'];
        
            $work = R::findOne('work', "url_title LIKE ?", array($ref));
            if ($work && $work->id > 0) {
                $this->view = 'details';
                $this->detailsAction($param);
                return;
            }
            $post = R::findOne('post', "url_title LIKE ?", array($ref));
            if ($post && $post->id > 0) {
                $this->view = 'post';
                $this->postAction($param);
                //toto set post anchor or something
                return;
            }
        }
        $this->view = 'work';
        $this->workAction($param);
    }
    
    public function emptyAction() {
        
    }

    private function loadContent($name) {
        $content = R::findone('content', 'name = ?', array($name));
        if ($content && $content->id > 0) {
            $this->view = "content";
            $this->content = $content->body;
        } else {
            $this->view = "500";
            $this->error = "Content pagina niet gevonden: ".$name;
        }
    }
    

}

?>
