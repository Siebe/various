<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of FormHelper
 *
 * @author User
 */
class FormHelper {
    //put your code here
    
    static public function field($name, $type, $value, $label = null, $error = null, $no_echo = false) {
        $result = '';
        if (!$value) {
            $value == '';
        }
        if ($label === null) {
            $label = ucfirst($name);
        }
        if ($label !== false && $type != 'hidden') {
            $result = '<label for="'.$name.'">'.$label.':</label><br>'."\n";
        }
        if ($error) {
            $result .='<p class="warning">'.$error.'</p>'."\n";
        }
        if ($type == 'textarea') {
            $result .='<textarea cols="80" rows="20" name="'.$name.'">'.$value.'</textarea>';
            
        } else {
            $result .= '<input type="'.$type.'" name="'.$name.'" value=';
            if ($type == 'checkbox') {
                $result .= '1';
                if ($value != '') {
                    $result .= ' checked="true"';
                }
            } else {
                $result .= '"'.$value.'"';
            }
            $result .= '>';
        }
        if ($type != 'hidden') {
            $result .= "<br>\n";
        }
        
        if ($type == 'file') {
            if ($value != '') {
                $result .= '<div class="image-preview">Huidige afbeelding:<br><img src="'.$value.'"></div>';
            } else {
                $result .= '<div class="image-preview">(nog geen afbeelding opgeslagen)</div>';
            }
        }
        if ($type != 'hidden') {
            $result .= "<br>\n";
        }
        
        if (!$no_echo) {
            echo $result;
        }
        return $result;
    }
    
    
    public static function form($name, $fields, $object, $validator_error = false, $no_echo = false) {
        $result = '<form name="'.$name.'" method="post" accept-charset="UTF-8" enctype="multipart/form-data">'."\n";
        foreach ($fields as $field) {
            if (!isset($field['name']) || !isset($field['type']) ||
                (!isset($field['value']) && !$object)) {
                $result .= "fout in form<br>";
                continue;
            }   
            $name = $field['name'];
            $type = $field['type'];
            $value = isset($field['value']) ? $field['value'] : $object->$name;
            $label = isset($field['label']) ? $field['label'] : false;
            $error = isset($validator_error[$name]) ? $validator_error[$name] : false;
            
            $result .= self::field($name, $type, $value, $label, $error, true);    
                
        }
        $result .= '<input type="submit" value="Versturen en Opslaan">'."\n</form><br>";

        if (!$no_echo) {
            echo $result;
        }
        return $result;
    }
}

?>
