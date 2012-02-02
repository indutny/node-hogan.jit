var core = exports,
    hogan = require('../hogan.jit');

core.compile = function compile(string) {
  return new hogan.binding.Template(string);
};
