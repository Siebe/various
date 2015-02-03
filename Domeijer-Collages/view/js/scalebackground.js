/* 
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */


function bg_scaler() {
    bg_left = document.getElementById('bg-left');
    bg_right = document.getElementById('bg-right');

    height = window.innerHeight; + 100;
    
    cur_height = bg_left.offsetHeight;
    
    if (height == cur_height) {
        return;
    }
    cur_width = bg_left.offsetWidth;
    
    width = parseInt((cur_width * (height/cur_height)), 10);
    
    bg_left.style.height = height + 'px';
    bg_right.style.height = height + 'px';
    bg_left.style.width = width + 'px';
    bg_right.style.width = width + 'px';
    
    
}

window.addEventListener('resize', function(event){
    bg_scaler();
});

window.addEventListener('onload', function(event){
    bg_scaler();
});