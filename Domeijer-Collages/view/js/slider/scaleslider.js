/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
var jssor_slider1;

function jssor_slider1_starter (containerId, width, height) {
    var options = {    
        
            $AutoPlay: true,
            $AutoPlayInterval: 7000,
            $SlideDuration: 1000,
            $LazyLoading: 1,
            $FillMode: 2,
            $DragOrientation: 0,
            $SlideWidth: width,
            $SlideHeight: height,
            $ArrowNavigatorOptions: {
                $Class: $JssorArrowNavigator$,
                $ChanceToShow: 2
            } 
            
        };
    container = document.getElementById(containerId);
    slide = document.getElementById('slides1');
    container.style.width = width+'px';
    container.style.height= height+'px';
    slide.style.width = width+'px';
    slide.style.height= height+'px';
    jssor_slider1 = new $JssorSlider$(containerId, options);
};

function jssor_slider1_scaler() {
    main = document.getElementById('content');
    if (main) {
        mainwidth = main.offsetWidth;
        mainheight = window.innerHeight;
        jssor_slider1.$ScaleWidth(mainwidth);
        //console.log(mainwidth+' x '+ mainheight);
    } else {
        //console.log("no main");
    }
}

function jssor_slider1_goto(id) {
    if (id > 0) {
        jssor_slider1.$GoTo(id-1);
    }
}

window.addEventListener('resize', function(event){
  jssor_slider1_scaler();
});

