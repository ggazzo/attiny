'use strict'

// class Genetic {
// 	constructor(params) {
// 		{
// 			this.evaluate, this.mutate
// 		} = params;
// 	}
// 	run() {
//     this._evaluate();
//     this._mutate();
//     this._evaluate();
//     this._reproduce();
// 	}
//   _reproduce(){
//     let tmp =this.pop.slice(0, 50);
//     this.pop = tmp.reduce(function(result, el) {
//       return tmp.reduce(function(result, el2) {
//         return result.push(this.reproduce(el, el2));
//       },result);
//     }, tmp);
//   }
//   _mutate(){
//     let tmp =this.pop.slice(0, 50);
//     this.pop = tmp.reduce(function(result, el) {
//       return result.concat(Array.apply(null, Array(10)).map(function() {
//     		return this.mutate(el);
//     	}));
//   	}, tmp);
//   }
// 	_evaluate() {
// 		this.pop.map(function(p) {
// 			el.erro = input.reduce(function(erro, el) {
// 				let resultado = this.evalute(p, el);
// 				return erro + Math.pow(y - resultado, 2);
// 			}, 0);
// 			return el;
// 		}).sort(function compareNumbers(a, b) {
// 			return a.erro - b.erro;
// 		});
// 	}
// }


// let a = new Genetic({
// 	evalute: (pop, input) - > {
// 		return pop.a * Math.pow(input.x, 4) + pop.b * Math.pow(input.x, 3) + pop.c * Math.pow(input.x, 2) + pop.d * input.x + pop.e;
// 	},
//   mutate:(el)-> {
//     return {
//       a: getRandomInt(el.a, .1),
//       b: getRandomInt(el.b, .4),
//       c: getRandomInt(el.c, .6),
//       d: getRandomInt(el.d, .8),
//       e: getRandomInt(el.e, 1)
//     }
//   },
//   reproduce : (el,el2)-> {
//     return {
//       a: Math.random() * 100 > 50 ? el.a : el2.a,
//       b: Math.random() * 100 > 50 ? el.b : el2.b,
//       c: Math.random() * 100 > 50 ? el.c : el2.c,
//       d: Math.random() * 100 > 50 ? el.d : el2.d,
//       e: Math.random() * 100 > 50 ? el.e : el2.e
//     }
//   }
// });
var jsonfile = require('jsonfile');

var file = 'data2.json';


let input = [
	[0.1,0,0,-3.4228],
	[2.3,0,0, 33.0576],
	[0.2,0,0, -2.8356],
	[0,0,0, -4],
	[3.5,0,0, 194.94],
	[2.5,0,0, 46.55],
	[10,0,0, 16984.7],
	[-4,0,0, 711.24],
	[4,0,0, 347],
	[-0.6,0,0, -5.9764],
	[-1.17,0,0,0.04598402]
];

var pop = Array.apply(null, Array(300)).map(function(el) {
	return {
    a: Math.random() * 10 - 5,
    a1: Math.random() * 10 - 5,
    a2: Math.random() * 10 - 5,
    a3: Math.random() * 10 - 5,
    a4: Math.random() * 10 - 5,
		b1: Math.random() * 10 - 5,
    b2: Math.random() * 10 - 5,
    b3: Math.random() * 10 - 5,
    b4: Math.random() * 10 - 5,
		c1: Math.random() * 10 - 5,
    c2: Math.random() * 10 - 5,
    c3: Math.random() * 10 - 5,
    c4: Math.random() * 10 - 5,
		d1: Math.random() * 10 - 5,
    d2: Math.random() * 10 - 5,
    d3: Math.random() * 10 - 5,
    d4: Math.random() * 10 - 5,
		e: Math.random() * 10 - 5
	}
});


function getRandomInt(original, peso) {
	if (Math.random() * 100 > 50) {
		return Math.random() * peso + original - peso * .5;
	}
	return original;
}

function _mutate(el) {
	return Array.apply(null, Array(10)).map(function() {
		return {
      a: getRandomInt(el.a, .1),

      a1: getRandomInt(el.a1, 1),
      a2: getRandomInt(el.a2, 1),
      a3: getRandomInt(el.a3, 1),
      a4: getRandomInt(el.a4, 1),

			b1: getRandomInt(el.b1, 4),
      b2: getRandomInt(el.b2, 4),
      b3: getRandomInt(el.b3, 4),
      b4: getRandomInt(el.b4, 4),

			c1: getRandomInt(el.c1, 6),
      c2: getRandomInt(el.c2, 6),
      c3: getRandomInt(el.c3, 6),
      c4: getRandomInt(el.c4, 6),

			d1: getRandomInt(el.d1, 8),
      d2: getRandomInt(el.d2, 8),
      d3: getRandomInt(el.d3, 8),
      d4: getRandomInt(el.d4, 8),

			e: getRandomInt(el.e, 1)
		}
	});
}

function mutate() {
	let original = pop.slice(0, 50);
	original = original.reduce(function(result, el) {
		let tmp = _mutate(el);
		return result.concat(tmp);
	}, original);

	pop = original;
}

function evaluate() {
	pop.forEach(function(el) {
		let {
			a,a1,a2,a3,a4, b1,b2,b3,b4, c1,c2,c3,c4, d1,d2,d3,d4, e
		} = el;
		// console.log(input);
		el.erro = input.reduce(function(erro, el) {
			let [x, y, z, res] = el;
			let resultado = a*(x+a1)*(x+b1)*(x+c1)*(x+d1)*(y+a2)*(y+b2)*(y+c2)*(y+d2)*(z+a3)*(z+b3)*(z+c3)*(z+d3)+e;
      //  a * Math.pow(x, 4) + b * Math.pow(x, 3) + c * Math.pow(x, 2) + d * x + e;
			return erro + Math.pow(res - resultado, 2);
		}, 0);
	});
	pop.sort(function compareNumbers(a, b) {
		return a.erro - b.erro;
	});
}

jsonfile.readFile(file, function(err, obj) {

	if (!err) {
		pop = obj;
	}
	for (let i = 0; i < 10000; i++) {
		evaluate();
		if (i % 1000 == 0) {
			console.log("epoca - ", pop[0].erro, i);
			console.log(`https://www.google.com.br/#q=` + encodeURIComponent(`plot ${pop[0].a}*x^4 + ${pop[0].b}*x^3 + ${pop[0].c}*x^2 + ${pop[0].d}*x + ${pop[0].e}`));
		}

		mutate();
	}
	jsonfile.writeFile(file, pop, function(err) {
		console.error(err);
	})
})
