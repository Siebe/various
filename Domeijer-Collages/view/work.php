<?php include 'header.php';?>


<div id="slider1_container">
    <!-- Slides Container -->
    <div id="slides1" data-u="slides">
<?php
        foreach($all_work as $work) {
            echo '
        <div class="the-slide">
            
            <a href="/'.$work->url_title.'">
                <div class="image-click">                                 
                    <div class ="small-text">Klik voor details</div>
                    <p>'.$work->title.'</p>

                </div>
            </a>
            <noscript>
                <img src="'.ImageHelper::preview($work->image).'" alt="'.$work->title.'" />
            </noscript>
            <img data-u="image" data-src="'.ImageHelper::preview($work->image).'" alt="'.$work->title.'" />
        </div>';
            
        }?>
        
    </div>
    
    <!-- Arrow Navigator Skin Begin -->
    <!-- Arrow Left -->
    <span data-u="arrowleft" class="jssora01l" style="width: 5%; height: 200px; top: 30%; left: 8px;">
    </span>
    <!-- Arrow Right -->
    <span data-u="arrowright" class="jssora01r" style="width: 5%; height: 200px; top: 30%; right: 8px">
    </span>
    <!-- Arrow Navigator Skin End -->
    
    <script> 
        jssor_slider1_starter('slider1_container', <?php echo Config::$slider_width ?>, <?php echo Config::$slider_height ?>);
        jssor_slider1_scaler();
        jssor_slider1_goto(<?php echo $start_index ?>);
    </script>
</div>

<?php include 'footer.php';?>    