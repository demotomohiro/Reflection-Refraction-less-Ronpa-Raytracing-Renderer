from jinja2 import Environment, FunctionLoader, ChoiceLoader, FileSystemLoader
from itertools import chain
from collections import OrderedDict

class TemplateError(Exception):
    def __init__(self, message):
        self.message = message

class _TemplateFunc:
    def __init__(self, *params):
        self.ids = params
        self.values = []

    def _getParamDict(self, *args):
        return OrderedDict((v,args[k]) for k,v in enumerate(self.ids))

    def addValues(self, *args):
        if len(args) != len(self.ids):
            raise TemplateError("Wrong number of template arguments")
        self.values.append(args)
        return self._getParamDict(*args)

    def getParamDictList(self):
        return list(self._getParamDict(*val) for val in self.values)

    def __str__(self):
        return "ids:" + str(self.ids) + "\nvalues:" + str(self.values)

def _tfrealID(name, paramdic):
    return name + "_" + "_".join(chain(*zip(paramdic.keys(), paramdic.values())))

class _TemplateFuncDict:
    def __init__(self):
        self.d = {}

    def define(self, name, *params):
        if name in self.d:
            raise TemplateError("Redefinition of template function \"" + name + "\"")
        self.d[name] = _TemplateFunc(*params)

    def call(self, name, *values):
        if name not in self.d:
            raise TemplateError(name + " is not defined")
        paramdic = self.d[name].addValues(*values)
        return _tfrealID(name, paramdic)

    def get(self, name):
        return self.d[name].getParamDictList()

def _load_internal_template_module(name):
    if name == "tmplFunc.tmpl":
        return """\
{% macro defTmplFunc(name) %}
    {% if GLSLJinja_IsInstanciate %}
        {% for d in _tfget(name) %}
            `caller(_tfrealID(name, d), *d.values())`
        {% endfor %}
    {% else %}
        `_tfdef(name, *varargs)`
    {% endif %}
{% endmacro %}
"""
    else:
        return None

class _PreprocessChoiceLoader(ChoiceLoader):
    def __init__(self, loaders):
        super().__init__(loaders)

    def get_source(self, environment, template):
        ret = super().get_source(environment, template)
        text = ret[0]
        lines = text.split("\n")
        lineNo = ["#line %d" % i for i in range(2, len(lines)+2)]
        #Last line under last "#line %d" must not empty or boost wave crash.
        newText = "\n".join(chain(*zip(lines, lineNo))) + "\n\n"
        return (newText, *ret[1:])

    def load(self, environment, name, globals=None):
        return super(ChoiceLoader, self).load(environment, name, globals)

class _GLSLJinaTempl:
    def __init__(self, templ):
        self.templ = templ

    def render(self, *args, **kwargs):
        tmplDic = _TemplateFuncDict()
        d = dict(
            _tfdef      = tmplDic.define,
            tfcall      = tmplDic.call,
            _tfget      = tmplDic.get,
            _tfrealID   = _tfrealID)
        self.templ.render(GLSLJinja_IsInstanciate = False, *args, **d, **kwargs)
        return self.templ.render(GLSLJinja_IsInstanciate = True, *args, **d, **kwargs)

class GLSLJinjaLoader:
    def __init__(self, searchpath=""):
        self.env = Environment(
                loader=_PreprocessChoiceLoader([
                    FunctionLoader(_load_internal_template_module),
                    FileSystemLoader(searchpath)]),
                variable_start_string="`", variable_end_string="`")

    def get_template(self, filename):
        return _GLSLJinaTempl(self.env.get_template(filename))

if __name__ == "__main__":
    def _tftest():
        tmplDic = _TemplateFuncDict()
        tmplDic.define("testFunc", "T", "U")
        tmplDic.define("testFunc2", "X", "Y", "Z")
        tmplDic.call("testFunc", "int", "vec3")
        tmplDic.call("testFunc", "float", "sometype")

        for k,v in tmplDic.d.items():
            print(k)
            print(v)
            d = v.getParamDictList()
            for j in d:
                print(j)

    _tftest()
    env = GLSLJinjaLoader()
    tmpl = env.get_template("testjinja2.tmpl")
    print(tmpl.render(delta="0.01", func="distanceFieldSphere"))
