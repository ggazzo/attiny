
'use strict'
var jsonfile = require('jsonfile');

var file = 'data.json';


let input = [
  [0.1,-3.4228],
  [2.3,33.0576],
  [0.2,-2.8356],
  [0,-4],
  [3.5,194.94],
  [2.5,46.55],
  [10,16984.7],
  [-4,711.24],
  [4,347],
  [-0.6, -5.9764],
  [-1.17, 0.04598402]
];

var pop = Array.apply(null, Array(300)).map(function(el) {
	return {
		a: Math.random() * 10 - 5,
		b: Math.random() * 10 - 5,
		c: Math.random() * 10 - 5,
		d: Math.random() * 10 - 5,
		e: Math.random() * 10 - 5
	}
});


function getRandomInt(original, peso) {
  if(Math.random()*100 > 80){
    return Math.random()*peso + original - peso*.5;
  }
  return original;
}

function _mutate(el){
  return Array.apply(null, Array(10)).map(function() {
    return {
      a:getRandomInt(el.a,.1),
      b:getRandomInt(el.b,.4),
      c:getRandomInt(el.c,.6),
      d:getRandomInt(el.d,.8),
      e:getRandomInt(el.e,1)
    }
  });
}

function mutate(){
  let original = pop.slice(0,50);
  original = original.reduce(function (result, el) {
    let tmp = _mutate(el);



    return result.concat(tmp);
  },original);

  pop = original;
}

function evaluate() {
	pop.forEach(function(el) {
		let {
			a, b, c, d, e
		} = el;
    // console.log(input);
		el.erro = input.reduce(function(erro, el) {

			let [x,y] = el;

			let resultado = a * Math.pow(x, 4) + b * Math.pow(x, 3) + c * Math.pow(x, 2) + d * x + e;
			return erro + Math.pow(y - resultado, 2);
		}, 0);
    });
    pop.sort(function compareNumbers(a, b) {
		return a.erro - b.erro;
	});
}

jsonfile.readFile(file, function(err, obj) {

    if(!err){ pop = obj;
    }
    for(let i = 0; i < 100000; i++){
      evaluate();
      if (i % 1000 == 0){ console.log("epoca - ",pop[0].erro,i);
      console.log(`https://www.google.com.br/#q=`+encodeURIComponent(`plot ${pop[0].a}*x^4 + ${pop[0].b}*x^3 + ${pop[0].c}*x^2 + ${pop[0].d}*x + ${pop[0].e}`));}

      mutate();
    }
    jsonfile.writeFile(file, pop, function (err) {
        console.error(err);
    })
})
