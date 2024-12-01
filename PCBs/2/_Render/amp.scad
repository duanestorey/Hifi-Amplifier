width = 435;
height = 82;
depth = 311;

margin_lr = 20;


$fn = 64;


difference() {
  cube( [ width, depth, height ] );
            
  translate( [ 2, 2, 2 ] ) {
    cube( [ width - 4, depth - 4, height + 10 ] );
  }
}



// new channel left (blue)
translate( [ 15, depth - margin_lr - 115, 10 ] ) {
  translate( [ 40, 2.5, 0 ] ) {
      cube( [ 55, 95, 4 ] );
  }
  color( "blue" )
  cube( [ 40, 100, 50 ] );   
}

// new channel right (blue)
translate( [ 190, depth - margin_lr - 115, 10 ] ) {
  translate( [ 40, 2.5, 0 ] ) {
      cube( [ 55, 95, 4 ] );
  }
  color( "blue" )
  cube( [ 40, 100, 50 ] );    
}

// adau
translate( [ 130, depth - margin_lr - 115, 10 ] ) {
    color( "pink" )
  cube( [ 45, 95, 4 ] );   
}

// SPDIF
translate( [ margin_lr, margin_lr, 10 ] ) {
  cube( [ 95, 55, 4 ] );   
}


// microprocessor (cyan)
translate( [ margin_lr, margin_lr + 70, 10 ] ) {
    color( "cyan" )
  cube( [ 95, 55, 4 ] );   
}

// subwoofer sel (magenta)
translate( [ 140, 90, 10 ] ) {
  color( "magenta" )
  cube( [ 65, 55, 4 ] );   
}

// channel sel (Yellow)
translate( [ 140, margin_lr, 10 ] ) {
  color( "yellow" )
  cube( [ 65, 55, 4 ] );   
}

// digital/ananlog power suppy
translate( [ 230, 90, 10 ] ) {
  color( "Lavender" )
  cube( [ 65, 55, 4 ] );   
}

// vinyl (RED)
translate( [ 230, margin_lr, 10 ] ) {
  color( "red" )
  cube( [ 65, 55, 4 ] );   
}




// power suppy
translate( [ width - margin_lr - 95, margin_lr, 10 ] ) {
  cube( [ 95, 95, 4 ] );   
}

// ground lift (green)
translate( [ width - margin_lr - 65, 130, 10 ] ) {
  color( "green" )
  cube( [ 65, 35, 4 ] );   
}


// toroid
translate( [ width - margin_lr - 95, depth - 135, 0 ] ) {
  cube( [ 95, 95, 45] );   
}
       
