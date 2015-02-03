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
class BackendActions {
    
    
    private $isPost = false;
    public $view;
    public $all_work;
    public $all_post;
    public $content;
    public $start_index;
    public $validation_error = false;
    
    public function __construct() {
        $this->isPost = $_SERVER['REQUEST_METHOD'] == "POST";
        $this->application = 'backend';
    }
        
    
    public function mainAction() {
        if ($this->authorize() == false) {
            return;
        }
        
    }
    
    public function workAction($param) {
        if ($this->authorize() == false) {
            return;
        } 

        $this->act = isset($param['act']) ? $param['act']: ''; 
        if ($this->act == 'edit'){
            $this->workEditAction($param);
            return;       
        }
        if ($this->act == 'touch'){
            $this->workTouchAction($param);
            return;       
        }
        if ($this->act == 'delete'){
            $this->workDeleteAction($param);
            return;       
        }
        //defailt list
        $this->workListAction($param);
        return;    
    }
    
    private function workListAction($param) {
        $this->view = "back_work_list";
        $this->all_work = R::findAll('work',"WHERE 1 ORDER BY updated_at DESC"); 
    }
    
    private function workEditAction($param) {
        $this->view = "back_work_edit";
        $this->work = OP::getObjectByIdOrTitle('work', $param, false);
        $this->work_is_stored = true;

        if ($this->isPost) {
            if ((!$this->work || $this->work->id < 1) && $_POST['id'] > 0) {
                $this->work = R::load('work', $_POST['id']);
            }
        }
        
        if (!$this->work || $this->work->id < 1) {
            $this->work = R::dispense('work');
            $this->work->created_at = date("Y-m-d H:m:s");
            $this->work->published = 1;
            $this->work_is_stored = false;
        }
        

        if ($this->isPost) {
            $title = $_POST['title'];
            $description = $_POST['description'];
            $published = isset($_POST['published']) ? 1 : 0;
            
            if ($this->work->title != $title) {
                $this->work->url_title = null;
            }
            
            $this->work->title = $title ? $title : $this->work->title;
            $this->work->url_title = $url_title = OP::getUrlTitle($this->work);
            $this->work->description = $description ? $description : $this->work->description;
            $this->work->published = $published;
            $this->work->updated_at = date("Y-m-d H:m:s");
            
            $image = ImageHelper::getAndSave($url_title ? $url_title: null, $this);
            $image_path = $image ? $image : $_POST['image_path'];
            
            if ($image === false && isset($this->image_error)) {
                $this->validation_error['image'] = $this->image_error;
                $this->work_is_stored = false;
                return;
            }
            if ($image === null ) {
                //maybe a path has previously been posted?
                $image = $image_path;
            }
            
            if (!$image && (!$this->work->image ||$this->work->image == '')) {
                $this->validation_error['image'] = "Geen afbeelding, wel nodig.";
            }
            $this->work->image = $image ? $image : $this->work->image;
            
            $this->validateForm(array('title' => 5, 'description' => 10));
            
            if (!$this->validation_error) {
                try {
                    R::store($this->work);
                    $this->work_is_stored = true;
                    $this->message = "Succesvol opgeslagen.";
                    if (!$this->work->published) {
                        $this->message .= " <b>Let op! Dit werk is nu niet te zien voor publiek.</b>";
                    }
                } catch (Exception $e) {
                    $this->view = 500;
                    $this->message = "Kon de content niet opslaan<br>".$e->getMessage();                    
                }   
            } else {
                $this->work_is_stored = false;
                $this->message = "Nog niet opgeslagen!";
            }
        }
        
        $this->form_fields = array (
            array ("name" => "title", "type" => "text", "label" => "Titel"),
            array ("name" => "description", "type" => "textarea", "label" => "Beschrijving"),
            array ("name" => "image", "type" => "file", "label" => "Afbeelding"),
            array ("name" => "published", "type" => "checkbox", "label" => "Publiek"),
            array ("name" => "image_path", "type" => "hidden", "value" => isset($image_path) ? $image_path : false),
            array ("name" => "id", "type" => "hidden", "value" => $this->work->id)
        );
    }    
    
    public function workTouchAction($param) {
        $work = OP::getObjectByIdOrTitle('work', $param, false);
        if (!$work) {
            $this->view = 500;
            $this->error = "Werk niet gevonden!";
            return;
        }
        $work->updated_at = date("Y-m-d H:m:s");
        R::store($work);
        $this->workListAction($param);
    }
    
    public function workDeleteAction($param) {
        if (!isset($param['reference'])) {
            $this->view = 404; 
            return;
        }
        $work = R::load('work', $param['reference']);
        if (!$work || $work->id < 1) {
            $this->error = "Oject niet gevonden!";
            $this->view = 500; 
            return;
        }
        
        $this->view = "back_delete";  
        $this->delete_model = "Werk";
        $this->delete_name = $work->title; 
        $this->redirect = "/backend/werk";
        
        try {
            $image = $work->image;
            $all_details = $work->xownDetailsList;
            R::trash($work);
            foreach($all_details as $details) {
                ImageHelper::delete($details->image, $this);
                R::trash($details);
            }
            ImageHelper::delete($image, $this);
            $this->message = "Succesvol verwijderd.";
        } catch (Exception $e) {  
            $this->message = "Kon niet verwijderd worden.<br>".$e->message;
        }
        
    }
       
    public function postAction($param) {
        if ($this->authorize() == false) {
            return;
        } 

        $this->act = isset($param['act']) ? $param['act']: '';    
        if ($this->act == 'edit'){
            $this->postEditAction($param);
            return;       
        }
        if ($this->act == 'touch'){
            $this->postTouchAction($param);
            return;       
        }
        if ($this->act == 'delete'){
            $this->postDeleteAction($param);
            return;       
        }
        
        //defailt list
        $this->postListAction($param);
        return;       
    
    }
    
    private function postListAction($param) {
        $this->view = "back_post_list";
        $this->all_post = R::findAll('post',"WHERE 1 ORDER BY created_at DESC"); 
    }
    
    private function postEditAction($param) {
        $this->view = "back_post_edit";
        $this->scripts[] = "/view/js/tinymce/tinymce.min.js";
        $this->post = OP::getObjectByIdOrTitle('post', $param, false);
        
        if ($this->isPost) {
            if ((!$this->post || $this->post->id < 1) && $_POST['id'] > 0) {
                $this->post = R::load('post', $_POST['id']);
            }
        }
        
        if (!$this->post || $this->post->id < 1) {
            $this->post = R::dispense('post');
            $this->post->created_at = date("Y-m-d H:m:s");
            $this->post->published = 1;
        }
        

        if ($this->isPost) {
            $title = $_POST['title'];
            $content = $_POST['content'];
            $published = isset($_POST['published']) ? 1 : 0;
            
            if ($this->post->title != $title) {
                $this->post->url_title = null;
            }
            
            $this->post->title = $title ? $title : $this->post->title;
            $this->post->url_title = OP::getUrlTitle($this->post);
            $this->post->content = $content ? $content : $this->post->content;
            $this->post->published = $published;
            $this->post->updated_at = date("Y-m-d H:m:s");
            
            
            $this->validateForm(array('title' => 5, 'content' => 10));
            
            if (!$this->validation_error) {
                try {
                    R::store($this->post);
                    $this->message = "Succesvol opgeslagen.";
                    if (!$this->post->published) {
                        $this->message .= " <b>Let op! Dit werk is nu niet te zien voor publiek.</b>";
                    }
                } catch (Exception $e) {
                    $this->view = 500;
                    $this->message = "Kon de content niet opslaan<br>".$e->getMessage();                    
                }   
            } else {
                $this->message = "Nog niet opgeslagen!";
            }
        }
        
        $this->form_fields = array (
            array ("name" => "title", "type" => "text", "label" => "Titel"),
            array ("name" => "content", "type" => "textarea", "label" => "Inhoud"),
            array ("name" => "published", "type" => "checkbox", "label" => "Publiek"),
            array ("name" => "id", "type" => "hidden", "value" => $this->post->id),
        );
    }
    
    public function postTouchAction($param) {
        $post = OP::getObjectByIdOrTitle('post', $param, false);
        if (!$post) {
            $this->view = 500;
            $this->error = "Item niet gevonden!";
            return;
        }
        $post->created_at = date("Y-m-d H:m:s");
        R::store($post);
        $this->postListAction($param);
    }
    
    public function postDeleteAction($param) {
        if (!isset($param['reference']) || !is_int($param['reference'])) {
            $this->view = 404; 
            return;
        }
        $post = R::load('post', $param['reference']);
        if (!$post || $post->id < 1) {
            $this->error = "Oject niet gevonden!";
            $this->view = 500; 
            return;
        }
        
        $this->view = "back_delete";  
        $this->delete_model = "Nieuws Item";
        $this->delete_name = $post->title; 
        $this->redirect = "/backend/nieuws";
         
        try {
            R::trash($post); 
            $this->message = "Succesvol verwijderd.";
        } catch (Exception $e) {  
            $this->message = "Kon niet verwijderd worden.<br>".$e->message;
        }
        
    }
    
    public function detailsAction($param) {
        if ($this->authorize() == false) {
            return;
        } 

        $this->act = isset($param['act']) ? $param['act']: '';
        if ($this->act == 'edit'){
            $this->detailsEditAction($param);
            return;       
        } 
        
        if ($this->act == 'touch'){
            $this->detailsEditAction($param);
            return;       
        }    
                
        if ($this->act == 'delete'){
            $this->detailsDeleteAction($param);
            return;       
        }      
        $this->view = 404;
    }   
    
    public function detailsEditAction($param) {
        $this->view = 'back_details_edit';
        $this->work = OP::getObjectByIdOrTitle('work', $param, false);
        if ($this->work == false) {
            $this->error = "Werk niet gevonden!";
            $this->view = 500;
            return;
        }
                
        if ($this->isPost) {
            $details = R::dispense('details');
            
            $image = ImageHelper::getAndSave($this->work->url_title.'_details_', $this);
            
            $details->created_at = date("Y-m-d H:m:s");
            
            if ($image === false && isset($this->image_error)) {
                return;
            }
       
            if (!$image && (!$this->work->image ||$this->work->image == '')) {
                $this->validation_error['image'] = "Geen afbeelding, wel nodig.";
            }
            
            $details->image = $image;
            $this->work->xownDetailsList[] = $details;
            
            if (!$this->validation_error) {
                try {
                    R::store($details);
                    R::store($this->work);
                    $this->message = "Succesvol opgeslagen.";
                } catch (Exception $e) {
                    $this->view = 500;
                    $this->message = "Kon de content niet opslaan<br>".$e->getMessage();                    
                }   
            } else {
                $this->message = "Nog niet opgeslagen!";
            }
            
        }
        
        $this->all_details = $this->work->xownDetailsList;
        
        $this->form_fields = array (
            array ("name" => "image", "type" => "file", "label" => "Afbeelding", "value" => "")
        );
        
        
    }
    
    public function detailsTouchAction($param) {
        $details = OP::getObjectByIdOrTitle('details', $param, false);
        if (!$details) {
            $this->view = 500;
            $this->error = "Item niet gevonden!";
            return;
        }
        $$details->created_at = date("Y-m-d H:m:s");
        R::store($details);
        $this->postEditAction(array());
    }
    
    public function detailsDeleteAction($param) {
        if (!isset($param['reference'])) {
            $this->view = 404; 
            return;
        }
        $details = R::load('details', $param['reference']);
        if (!$details || $details->id < 1) {
            $this->error = "Oject niet gevonden!";
            $this->view = 500; 
            return;
        }
        
        $this->view = "back_delete";  
        $this->delete_model = "Extra afbeelding behorend bij ";
        $this->delete_name = $details->work->title; 
        $this->redirect = "/backend/details/edit/".$details->work->id;
         
        try {
            $image = $details->image;
            R::trash($details); 
            ImageHelper::delete($image, $this);
            $this->message = "Succesvol verwijderd.";
        } catch (Exception $e) {  
            $this->message = "Kon niet verwijderd worden.<br>".$e->message;
        }
        
    }
    
    public function contactAction($param) {
        $this->contentEditAction($param, 'contact', 'Contact');
    }

    public function aboutAction($param) {
         $this->contentEditAction($param, 'about', 'Over');
    }    

    public function contentEditAction($param, $content_type, $content_name = false) {
        if ($this->authorize() == false) {
            return;
        }
        $this->name = $content_type;
        $this->scripts[] = "/view/js/tinymce/tinymce.min.js";
        $this->view = "back_content_edit";
        $this->content = R::findOne('content', "WHERE name LIKE ?", array($content_type));
        $this->content_name = $content_name ? $content_name : $content_type;
        
        if (!$this->content || $this->content->id < 1) {
            $this->view = 500;
            $this->message = "Content niet gevonden in database: ".$content_type;
            return;
        }
         
        if ($this->isPost) {
            
            $body = $_POST['body'];
            $this->content->body = $body ? $body : $this->content->body;
            if ($this->validateForm(array('body' => 10))) {
                try {
                    R::store($this->content);
                    $this->message = "Succesvol opgeslagen";
                } catch (Exception $e) {
                    $this->view = 500;
                    $this->message = "Kon de content niet opslaan<br>".$e->getMessage();                    
                }   
            } 
            
        }
        
        $this->form_fields = array (
            array("name" => "body", "type" => "textarea", "label" => "Inhoud")
        );
        
    }
       
    public function logoutAction() {
        Auth::logOut();
        header("Location: /");
        die('Redirecting...'); //yes, a die(), after a redirect so it's ok, ok?
    }
    
    
    private function authorize() {
        if (Auth::isAuth()) {
            $this->user = Auth::getUser();
            $this->username = $this->user->username;
            return true;
        }     
        if ($this->isPost) {
            $name = $_POST['username'];
            $password = $_POST['password'];
            if ($name && $password && 
                $name != '' && $password != '' &&
                Auth::AuthUser($name, $password)
            ) { 
                $this->user = Auth::getUser();
                $this->username = $this->user->username;
                return true;
            }
            $this->message = "Inloggen mislukt";
        }
        $this->view = "back_login";
        return false;
    }
    
    private function validateForm($fields) {
        $this->validation_error = false;
        foreach ($fields as $field => $min_length) {
            if ($min_length < 1){
                continue;
            }
            if (!isset($_POST[$field]) || strlen($_POST[$field]) < $min_length) {
                $this->validation_error[$field] = "Minimaal $min_length karakter(s)"; 
            }
        }
        if ($this->validation_error === false) {
            return true;
        }
        return false;        
    }
    

}

?>
