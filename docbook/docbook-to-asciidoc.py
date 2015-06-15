#! /usr/bin/python 
import xml.etree.ElementTree as ET
import re
import sys
import getopt

toplevel_doc = "../../"

def textfold(s):
    a = ""
    o = ""
    for w in s.split(" "):
        if (len(o) > 0) and ((len(o) + len(w) + 1) > 72):
            if a != "":
                a = a + "\n" + o
            else:
                a = o
            o = ""
        if len(o) > 0:
            o = o + " " + w
        else:
            o = w
    if a != "":
        a = a + "\n" + o
    else:
        a = o
    return a

def strip_space(s):
    return re.sub(r'\s+', '', s.strip())

def normalize(s):
    return re.sub(r'\s+', ' ', s)

def replaceable(el, literal=False):
    role = el.attrib.get('role')
    if role == None:
        if literal:
            return el.text
        else:
            return "'" + normalize(el.text) + "'"
    elif role == 'entity':
        t = el.text.strip()
        if t == 'version':
            return "6.0"
        elif t == 'shortversion':
            return "6"
        elif t == 'previousversion':
            return "5.2"
    else:
        raise Exception("Unknown role: " + role)

def informaltitle(el):
    t = "\n."
    if el.text:
        t += el.text
    for child in el:
        if child.tag == 'replaceable':
            t += replaceable(child)
        elif child.tag == 'command':
            t += command(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail is not None:
            t += child.tail

    if t == '\n.':
        return ""
    return "\n" + normalize(t.strip())

def title(el, level):
    levelstring = "=" * level
    t = levelstring + " "
    if el.text is not None:
        t += el.text
    for child in el:
        if child.tag == 'replaceable':
            t += replaceable(child)
        elif child.tag == 'command':
            t += command(child)
        elif child.tag == 'filename':
            t += filename(child)
        elif child.tag == 'option':
            t += option(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail is not None:
            t += child.tail
    t += " " + levelstring
    return normalize(t) + "\n\n"

def simpara(el):
    s = ""
    if el.text is not None:
        s += normalize(el.text.lstrip())
    for child in el:
        if child.tag == 'filename':
            s += filename(child)
        elif child.tag in [ 'command', 'application', 'keycombo', 'function', 'systemitem', 'package'  ]:
            s += command(child)
        elif child.tag == 'literal':
            s += literal(child)
        elif child.tag == 'emphasis':
            s += emphasis(child)
        elif child.tag in ['replaceable', 'option', 'parameter']:
            s += replaceable(child)
        elif child.tag == 'computeroutput':
            s += literal(child)
        elif child.tag == 'ulink':
            s += ulink(child)
        elif child.tag in ['envar', 'varname']:
            s += envar(child)
        elif child.tag == 'olink':
            s += olink(child)
        elif child.tag == 'glossterm':
            s += child.text
        elif child.tag == 'xref':
            s += link(child)
        elif child.tag == 'citerefentry':
            s += citerefentry(child)
        else:
            raise Exception(child.tag + " in simpara")
        if child.tail is not None and child.tail.strip() != "":
            s += normalize(child.tail)
    return s

def blockquote(el):
    q = "\n" + "*" * 71 + "\n"
    if el.text is not None:
        q += el.text.lstrip()
    for child in el:
        if child.tag == 'simpara':
            q += simpara(child)
        elif child.tag == 'para':
            q += para(child)
        elif child.tag == 'itemizedlist':
            q += itemizedlist(child)
        elif child.tag == 'screen':
            q += screen(child)
        else:
            raise Exception(child.tag + " in " + el.tag)


    q += "\n" + "*" * 71 + "\n"

    return q

def listitem(el, depth, bullet="*"):
    i = bullet * depth + " "
    if el.text is not None:
        i += normalize(el.text.strip())
    firstel = True
    for child in el:
        if not firstel:
            i += "+\n"
        firstel = False
        if child.tag == 'simpara':
            i += simpara(child).strip() + "\n"
        elif child.tag == 'para':
            i += para(child, inlist=True).strip() + "\n"
        elif child.tag == 'itemizedlist':
            i += itemizedlist(child, depth+1)
        elif child.tag == 'tip':
            i += tip(child)
        elif child.tag == 'important':
            i += important(child)
        elif child.tag == 'warning':
            i += warning(child)
        elif child.tag == 'screen':
            i += screen(child)
        elif child.tag == 'blockquote':
            i += blockquote(child)
        elif child.tag == 'note':
            i += note(child)
        else:
            raise Exception(child.tag + " in listitem")
    return i

def itemizedlist(el, depth=1):
    l = "\n"
    for child in el:
        if child.tag == 'listitem':
            l += listitem(child, depth) + "\n"
    return l

def orderedlist(el, depth=1):
    l = ""
    for child in el:
        if child.tag == 'listitem':
            l += listitem(child, depth, bullet=".") + "\n"
        else:
            raise Exception(child.tag  + " in " + el.tag)
    return l

def envar(el):
    p = "++"
    if el.text:
        p += normalize(el.text)
    p += "++"
    return p

def filename(el):
    p = "++"
    if el.text:
        p += normalize(el.text)
    for child in el:
        if child.tag == 'envar':
            if child.text:
                p += normalize(child.text)
        elif child.tag == 'varname':
            if child.text:
                p += normalize(child.text)
        elif child.tag == 'replaceable' and \
             child.attrib.get("role") == "entity":
            p += replaceable(child)
        elif child.tag == 'replaceable':
            if child.text:
                p += normalize(child.text)
        else:
            raise Exception(child.tag + " in filename")
        if child.tail is not None:
            p += normalize(child.tail)
    p += "++"
    if el.tail:
        p += normalize(el.tail)
    return p

def literal(el):
    p = "++"
    if el.text:
        p += normalize(el.text)
    p += "++"
    return p

def command(el):
    c = ''
    if el.text is not None:
        c = "**++" + normalize(el.text) + "++**"

    return c

def classname(el):
    c = ''
    if el.text is not None:
        c = "++" + normalize(el.text) + "++"
    return c

def option(el):
    return "'" + el.text + "'"
def olink(el):
    targetdoc = el.attrib.get("targetdoc")
    targetptr = el.attrib.get("targetptr")
    o = "link:"

    if targetdoc is not None:
        suffixmaps = {
            "Admin": "admin/index.html",
            "Developer": "developer/index.html",
            "Key": "key/index.html",
            "Mig": "mig/index.html",
            "PI": "pi/index.html",
            "QP": "qp/index.html",
            "RN": "rn/index.html",
            "User": "user/index.html"
        }
        found = False
        for s in suffixmaps.keys():
            if not(found) and targetdoc.endswith(s):
                prefix = targetdoc.replace(s, "")
                o += toplevel_doc + prefix + "/" + suffixmaps[s]
                found = True
        if not found:
            if targetdoc == 'gtadmin':
                o += toplevel_doc + "admin/install/index.html"
            elif targetdoc == 'quickstart':
                o += toplevel_doc + "admin/quickstart/index.html"
            elif targetdoc == 'gtadminappendix':
                o += toplevel_doc + "admin/install/appendix.html"
            elif targetdoc == 'gtcommands':
                o += toplevel_doc + "appendices/commands/index.html"
            elif targetdoc == 'gtdeveloper':
                o += toplevel_doc + "appendices/developer/index.html"
            elif targetdoc in ['gridftp', 'gram5', 'gsic', 'ccommonlib', 'xio', 'myproxy', 'gsiopenssh', 'simpleca']:
                o += toplevel_doc + targetdoc + "/index.html"
            elif targetdoc == 'gram5':
                o += toplevel_doc + "gram5/index.html"
            elif targetdoc == 'rn':
                o += toplevel_doc + "rn/index.html"
            else:
                raise Exception("targetdoc: " + targetdoc)
    if targetptr is not None:
        o += "#" + targetptr

    if el.text is not None:
        o += "[" + normalize(el.text.strip())
    else:
        o += "["


    for child in el:
        if child.tag == 'command':
            o += command(child)
        elif child.tag == 'classname':
            o += classname(child)
        elif child.tag == 'replaceable':
            o += replaceable(child)
        elif child.tag == 'option':
            o += option(child)
        else:
            raise Exception(child.tag + " in olink")
        if child.tail is not None and child.tail.strip() != "":
            o += normalize(child.tail)
    o += "]"
    return o

def screen(el):
    s=""
    if el.text is not None and el.text.strip() != '':
        s += el.text
    for child in el:
        if child.tag in ['command', 'computeroutput', 'userinput', 'option', 'replaceable', 'literal', 'emphasis', 'olink']:
            if child.text is not None:
                s += child.text
            if child.tail is not None:
                s += child.tail
        elif child.tag == 'xref':
            s += link(child)
        elif child.tag == 'prompt':
            s += prompt(child, True)
        elif child.tag == '{http://www.w3.org/2001/XInclude}include':
            s += include(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail is not None:
            s += child.tail
    s = re.sub("\n-", "\n -", s)
    return "--------\n" + s + "\n--------\n"

def ulink(el):
    u = ""
    url = el.attrib['url'].replace("_", "%5f").replace("${version}", "6.0")
    if url.startswith("http"):
        u = url
    else:
        u = "link:" + el.attrib['url']
        
    if el.text is not None:
        u += "[" + normalize(el.text.strip()) + "]"
    return u

def link(el):
    l = "<<" + el.attrib['linkend'] + ">>"
        
    if el.text is not None:
        l += "[" + normalize(el.text.strip()) + "]"
    return l


def emphasis(el):
    e = "**" + normalize(el.text)
    for child in el:
        if child.tag == 'replaceable':
            e += replaceable(child)
        elif child.tag == 'olink':
            e += olink(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail is not None and child.tail.strip() != "":
            e += normalize(child.tail)

    e += "**"
    return e

def citerefentry(el):
    t = el.find("refentrytitle").text
    v = '1'
    if el.find("manvolnum") is not None:
        v = el.find("manvolnum").text

    return "++" + t + "(" + v + ")++"


def para(el, fold=True, formal=False, inlist=False):
    p = ""
    unfolded = ''

    elid = el.attrib.get('id')
    if elid is not None:
        anchor = "\n[[" + strip_space(elid) + "]]\n"
        p += anchor

    if formal and el.find('title') is not None:
        p += informaltitle(el.find('title'))

    cc = "\n\n"
    if inlist:
        cc = "\n+\n"

    if el.text is not None:
        unfolded += normalize(el.text).lstrip()
    for child in el:
        if child.tag == 'emphasis':
            unfolded += emphasis(child)
        elif child.tag == 'title':
            pass
        elif child.tag ==  'classname':
            unfolded += classname(child)
        elif child.tag in [ 'command', 'application', 'systemitem', 'keycombo', 'function', 'package', 'keycap', 'classname', 'hardware', 'code', 'methodname', 'varname', 'type']:
            unfolded += command(child)
        elif child.tag == 'filename':
            unfolded += filename(child)
        elif child.tag == 'option' or child.tag == 'parameter':
            unfolded += "'" + normalize(child.text) + "'"
        elif child.tag == 'replaceable':
            unfolded += replaceable(child)
        elif child.tag == 'itemizedlist':
            if fold:
                p += textfold(unfolded) + "\n" + itemizedlist(child, 1) + "\n"
                unfolded = ''
            else:
                unfolded += "\n" + itemizedlist(child, 1) + "\n"
        elif child.tag == 'orderedlist':
            if fold:
                p += textfold(unfolded) + "\n" + orderedlist(child, 1) + "\n"
                unfolded = ''
            else:
                unfolded += "\n" + itemizedlist(child, 1) + "\n"
        elif child.tag == '{http://www.w3.org/2001/XInclude}include':
            unfolded += "\n" + include(child) + "\n"
        elif child.tag == 'para':
            unfolded += "\n" + para(child) + "\n"
        elif child.tag == 'olink':
            unfolded += olink(child)
        elif child.tag in ['literal', 'computeroutput', 'envar', 'userinput', 'structfield', 'structname']:
            unfolded += literal(child)
        elif child.tag == 'glossterm':
            unfolded += child.text
        elif child.tag == 'citerefentry':
            unfolded += citerefentry(child)
        elif child.tag in [ 'screen', 'programlisting']:
            if fold:
                p += textfold(unfolded) + cc + screen(child)
                unfolded = ''
            else:
                unfolded += cc + screen(child)
        elif child.tag == 'variablelist':
            if fold:
                p += textfold(unfolded) + cc + variablelist(child)
                unfolded = ''
            else:
                unfolded += cc + variablelist(child)
        elif child.tag == 'example':
            if fold:
                p += textfold(unfolded) + example(child)
                unfolded = ''
            else:
                unfolded += example(child)
        elif child.tag == 'informaltable':
            if fold:
                p += textfold(unfolded) + informaltable(child)
                unfolded = ''
            else:
                unfolded += informaltable(child)
        elif child.tag == 'table':
            if fold:
                p += textfold(unfolded) + table(child)
                unfolded = ''
            else:
                unfolded += table(child)
        elif child.tag == 'email':
            unfolded += child.text
        elif child.tag == 'note':
            if fold:
                p += textfold(unfolded) + note(child)
                unfolded = ''
            else:
                unfolded += note(child)
        elif child.tag == 'important':
            if fold:
                p += textfold(unfolded) + important(child)
                unfolded = ''
            else:
                unfolded += note(child)
        elif child.tag == 'tip':
            if fold:
                p += textfold(unfolded) + tip(child)
                unfolded = ''
            else:
                unfolded += note(child)
        elif child.tag == 'warning':
            if fold:
                p += textfold(unfolded) + warning(child)
                unfolded = ''
            else:
                unfolded += note(child)
        elif child.tag == 'ulink':
            unfolded += ulink(child)
        elif child.tag == 'link':
            unfolded += link(child)
        elif child.tag == 'blockquote':
            unfolded += blockquote(child)
        elif child.tag == 'indexterm':
            unfolded += indexterm(child)
        else:
            raise Exception(child.tag + " in para")
        if child.tail is not None:
            unfolded += normalize(child.tail)
    if fold:
        p += textfold(unfolded)
    else:
        p += unfolded
    if p[-2:] != "\n\n":
        p += "\n"
    return p

    
def admonpar(el, admon):
    i = "[" + admon + "]\n--\n"

    if el.text is not None:
        i += normalize(el.text.strip())

    for child in el:
        if child.tag == 'para':
            i += para(child)
        elif child.tag == 'simpara':
            i += simpara(child)
        elif child.tag == 'screen':
            i += screen(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail is not None and child.tail.strip() != "":
            i += normalize(child.tail)
    i += "\n--\n"
    return i

def important(el):
    return admonpar(el, "IMPORTANT")

def note(el):
    return admonpar(el, "NOTE")

def tip(el):
    return admonpar(el, "TIP")

def warning(el):
    return admonpar(el, "WARNING")

def indexterm(el):
    terms = []
    prim = el.find("primary")
    if prim is not None and prim.text is not None:
        terms.append(prim.text)
    sec = el.find("secondary")
    if sec is not None and sec.text is not None:
        terms.append(sec.text)
    tert = el.find("tertiary")
    if tert is not None and tert.text is not None:
        terms.append(tert.text)

    return "indexterm:[" + ",".join(terms) + "]\n"

def section(el, level=3):
    s = "\n"
    elid = el.attrib.get('id')
    titleabbrev = el.find("./titleabbrev")
    if elid is not None:
        anchor = "[[" + strip_space(elid)
        if titleabbrev is not None:
            anchor += "," + titleabbrev.text
        anchor += "]]\n"
        s += anchor
    for child in el:
        if child.tag == 'title':
            s += title(child, level)
        elif child.tag == 'section':
            s += section(child, level + 1)
        elif child.tag == 'para':
            s +=  para(child) + "\n"
        elif child.tag == 'simpara':
            s +=  simpara(child) + "\n"
        elif child.tag == 'informaltable':
            s += informaltable(child)
        elif child.tag == 'itemizedlist':
            s += "\n\n" + itemizedlist(child, 1) + "\n"
        elif child.tag == 'orderedlist':
            s += "\n\n" + orderedlist(child, 1) + "\n"
        elif child.tag == 'example':
            s += "\n\n" + example(child) + "\n"
        elif child.tag in [ 'screen', 'programlisting']:
            s += "\n\n" + screen(child) + "\n"
        elif child.tag == 'important':
            s += important(child)
        elif child.tag == 'warning':
            s += warning(child)
        elif child.tag == '{http://www.w3.org/2001/XInclude}include':
            s += include(child)
        elif child.tag == 'table':
            s += table(child)
        elif child.tag == 'note':
            s += note(child)
        elif child.tag == 'figure':
            s += figure(child)
        elif child.tag in [ 'simplesect', 'refsection', 'refsect2', 'refsect3']:
            s += section(child) + "\n"
        elif child.tag == 'variablelist':
            s += "\n\n" + variablelist(child) + "\n"
        elif child.tag == 'productionset':
            s += "\n\n" + productionset(child) + "\n"
        elif child.tag == 'blockquote':
            s += blockquote(child)
        elif child.tag == 'mediaobject':
            s += mediaobject(child)
        elif child.tag == 'formalpara':
            s += para(child, formal=True)
        elif child.tag == 'indexterm':
            s += indexterm(child)
        else:
            raise Exception(child.tag + " in section")
        if child.tail is not None and child.tail.strip() != "":
            s += normalize(child.tail)
    return s

def production(el, n):
    p = "| "
    #if el.attrib.get("id"):
    #    p += "[[" + el.attrib["id"] + "]] "
    #p += "[" + str(n) + "] | "
    p += "'" + el.find("./lhs").text + "' | "

    rhs = el.find("./rhs")
    rhs_out = ""
    t = normalize(rhs.text.strip())

    while t.find(" |") != -1:
        i = t.find(" |")
        r = t[0:i]
        t = t[i+2:]
        for w in r.split(" "):
            if len(w) > 0:
                rhs_out += "`" + w.strip().replace("|", "\\|") + "` "
        rhs_out += " +\n\\|"
    for w in t.split(" "):
        if len(w) > 0:
            rhs_out += "`" + w.strip().replace("|", "\\|") + "` "

    annotations = ""

    for l in rhs:
        if l.tag == 'nonterminal':
            # anchors not working table entry
            #if l.attrib.get("def") is not None:
            #    rhs_text += " +\nlink:" + normalize(l.attrib["def"])+" "
            rhs_out += " '"+normalize(l.text.strip())+"'"
        elif l.tag == 'lineannotation':
            annotations += l.text + " +\n";
        if l.tail is not None:
            t = normalize(l.tail).strip()
            if t.startswith("|"):
                rhs_out += " +\n\\|"
                t = t[1:]
            if re.search(r"\s+\|", t):
                for r in re.split(r"\s+\)", t):
                    if r is not None:
                        for w in re.split(" ", r):
                            if len(w) > 0:
                                rhs_out += " `" + w.strip().replace("|", "\\|") + "` "
            elif len(t.strip()) > 0:
                rhs_out += " `" + t.strip().replace("|", "\\|")  + "`"

    p += rhs_out + " | " + annotations + "\n"
    return p

def productionset(el):
    p = ""
    if el.find("./title") is not None:
        p += informaltitle(el.find("./title"))

    n = 1
    p += "\n[cols=3,options='header']\n"
    p += "|" + "=" * 71 + "\n"
    p += "| Production | Rule | Annotations\n"
    for child in el:
        if child.tag == 'title':
            pass
        elif child.tag == 'production':
            p += production(child, n)
            n = n + 1
        else:
            raise Exception(child.tag + " in " + el.tag)
    p += "|" + "=" * 71
    return p

def mediaobject(el):
    m = ""
    image = el.find(".//imagedata")
    if image is not None:
        m += "image::" + image.attrib.get('fileref') + "["
        m += "scaledwidth=\"75%\""
        if image.attrib.get("align") is not None:
            m += ",align=\"" + image.attrib.get("align") + "\""
        m += "]"
    return m

def figure(el):
    f = ""
    delim = ""
    elid = el.attrib.get('id')
    if elid is not None:
        f += "\n[[" + strip_space(elid) + "]]\n"
    for child in el:
        if child.tag == 'title':
            f += informaltitle(child)
            delim = "\n--\n"
            f += delim
        elif child.tag == 'mediaobject':
            f += mediaobject(child)
        elif child.tag == 'programlisting':
            f += screen(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
    f += delim
    return f


def example(el):
    elid = el.attrib.get('id')
    titleabbrev = el.find("./titleabbrev")
    e = ""
    if elid is not None:
        anchor = "\n[[" + strip_space(elid)
        if titleabbrev is not None:
            anchor += "," + titleabbrev.text
        anchor += "]]\n"
        e += anchor
    for child in el:
        if child.tag == 'title':
            e += informaltitle(child)
    e += "\n" + "=" * 71 + "\n"
    for child in el:
        if child.tag == 'title':
            pass
        elif child.tag == 'para':
            e += para(child)
        elif child.tag == 'screen':
            e += screen(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail is not None and child.tail.strip() != "":
            e += normalize(child.tail)
    e += "\n" + "=" * 71 + "\n"
    return e
    
def chapter(el, level=2):
    elid = el.attrib.get('id')
    titleabbrev = el.find("./titleabbrev")
    c = ""
    seenPara = False
    if elid is not None:
        anchor = "\n[[" + strip_space(elid)
        if titleabbrev is not None:
            anchor += "," + titleabbrev.text
        anchor += "]]\n"
        c += anchor
    for child in el:
        if child.tag not in [ 'para', 'itemizedlist', 'screen', 'note' ] and seenPara:
            c += "\n--\n"
            seenPara = False
        if child.tag == 'title':
            c += title(child, level)
        elif child.tag == 'para':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += para(child) + "\n"
        elif child.tag == 'simpara':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += simpara(child) + "\n"
        elif child.tag == 'itemizedlist':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += itemizedlist(child) + "\n"
        elif child.tag == 'orderedlist':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += orderedlist(child) + "\n"
        elif child.tag == 'variablelist':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += variablelist(child) + "\n"
        elif child.tag == 'screen':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += screen(child) + "\n"
        elif child.tag == 'note':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += note(child) + "\n"
        elif child.tag == 'important':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += important(child) + "\n"
        elif child.tag == 'section':
            c += section(child, level+1)
        elif child.tag == 'blockquote':
            c += blockquote(child)
        elif child.tag == 'indexterm':
            pass
        elif child.tag == '{http://www.w3.org/2001/XInclude}include':
            first_letter = re.search(r"([a-zA-Z])", child.attrib['href']).group(0)
            if first_letter != first_letter.upper():
                c += "\n:leveloffset: 2\n"
            c += include(child)
            if first_letter != first_letter.upper():
                c += "\n:leveloffset: 0\n"
        elif child.tag == 'titleabbrev':
            pass
        else:
            raise Exception(child.tag + " in chapter")
    if seenPara:
        c += "\n--\n"
    return c

def part(el, level=2):
    elid = el.attrib.get('id')
    titleabbrev = el.find("./titleabbrev")
    c = "\n"
    seenPara = False
    if elid is not None:
        anchor = "\n[[" + strip_space(elid)
        if titleabbrev is not None:
            anchor += "," + titleabbrev.text
        anchor += "]]\n"
        c += anchor
    for child in el:
        if child.tag not in [ 'para', 'itemizedlist', 'screen', 'note' ] and seenPara:
            c += "\n--\n"
            seenPara = False
        if child.tag == 'title':
            c += title(child, level)
        elif child.tag == 'para':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += para(child) + "\n"
        elif child.tag == 'itemizedlist':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += itemizedlist(child) + "\n"
        elif child.tag == 'screen':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += screen(child) + "\n"
        elif child.tag == 'note':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += note(child) + "\n"
        elif child.tag == 'section':
            c += section(child, level+1)
        elif child.tag == 'chapter':
            c += chapter(child, level+1)
        elif child.tag == 'indexterm':
            pass
        elif child.tag == '{http://www.w3.org/2001/XInclude}include':
            first_letter = re.search(r"([a-zA-Z])", child.attrib['href']).group(0)
            if first_letter != first_letter.upper():
                c += "\n:leveloffset: " + str(level) + "\n"
            c += include(child)
            if first_letter != first_letter.upper():
                c += "\n:leveloffset: 0\n"
        elif child.tag == 'titleabbrev':
            pass
        else:
            raise Exception(child.tag + " in chapter")
    if seenPara:
        c += "\n--\n"
    return c
def appendix(el):
    elid = el.attrib.get('id')
    titleabbrev = el.find("./titleabbrev")
    c = ""
    seenPara = False
    if elid is not None:
        anchor = "\n[[" + strip_space(elid)
        if titleabbrev is not None:
            anchor += "," + titleabbrev.text
        anchor += "]]\n"
        c += anchor
    for child in el:
        if child.tag != 'para' and child.tag != 'itemizedlist' and child.tag != 'screen' and seenPara:
            c += "\n--\n"
            seenPara = False
        if child.tag == 'title':
            c += title(child, 2)
        elif child.tag == 'titleabbrev':
            pass
        elif child.tag == 'para':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += para(child) + "\n"
        elif child.tag == 'itemizedlist':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += itemizedlist(child) + "\n"
        elif child.tag == 'screen':
            if not seenPara:
                c += "\n--\n"
                seenPara = True
            c += screen(child) + "\n"
        elif child.tag == 'section':
            c += section(child, 3)
        elif child.tag == 'indexterm':
            pass
        elif child.tag == '{http://www.w3.org/2001/XInclude}include':
            c += include(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
    if seenPara:
        c += "\n--\n"
    return c

def tgroup(el):
    t = ""
    cols = int(el.attrib['cols'])
    for child in el:
        if child.tag == 'tbody':
            t += tbody(child, cols)
        elif child.tag == 'thead':
            t += tbody(child, cols)
        else:
            raise Exception(child.tag + " in tgroup")
        if child.tail is not None and child.tail.strip() != "":
            t += normalize(child.tail)
    return t

def pass_through(el):
    t = "<" + el.tag
    for k in el.attrib:
        t += " " + k + "='" + el.attrib[k] + "'"
    t += ">"
    if el.text is not None:
        t += el.text
    for child in el:
        t += pass_through(child)
        if child.tail is not None:
            t += child.tail
    t += "</" + el.tag + ">"
    return t

def informaltable(el):
    t = "\n\n" + "+" *71 + "\n"
    t += pass_through(el)
    t += "\n" + "+" *71 + "\n\n"
    return t

def entry(el):
    e = "| "
    if el.text != None:
        e += el.text
    for child in el:
        if child.tag == 'para':
            e += para(child, False).replace("|", "\\|")
        elif child.tag == 'glossterm':
            e += child.text
        elif child.tag == 'simpara':
            e += simpara(child).replace("|", "\\|")
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail:
            e += child.tail
    return e

def row(el, cols):
    r = ""
    for child in el:
        if child.tag == 'entry':
            r += entry(child)
    return r

def tbody(el, cols):
    t = ""
    for child in el:
        if child.tag == 'row':
            t += row(child, cols)
        else:
            raise Exception(child.tag + " in tbody")
    return t

def table(el):
    t = "\n\n" + "+" *71 + "\n"
    t += pass_through(el)
    t += "\n" + "+" *71 + "\n\n"
    return t

def abstract(el):
    seenPara = False
    a = ""
    for child in el:
        if child.tag == 'title':
            a += informaltitle(child)
        elif child.tag == 'bookinfo':
            a += bookinfo(child)
        elif child.tag == 'para':
            if not seenPara:
                a += "\n--\n"
                seenPara = True
            a += para(child)
        elif child.tag == 'simpara':
            if not seenPara:
                a += "\n--\n"
                seenPara = True
            a += simpara(child)
        else:
            raise Exception(child.tag + " in abstract")
        if child.tail and child.tail.strip() != "":
            a += normalize(child.tail)
    if seenPara:
        a += "\n--\n\n"
    return a
            
    
def bookinfo(el):
    i = ""
    elid = el.attrib.get('id')
    if elid is not None:
        i += "\n[[" + strip_space(elid) + "]]\n"
    for child in el:
        if child.tag == 'abstract':
            i += abstract(child)
        elif child.tag == 'title':
            i += title(child, 1)
        else:
            raise Exception(child.tag + " in bookinfo")
        if child.tail:
            i += normalize(child.tail)
    return i
    
def include(el):
    return "\ninclude::" + el.attrib['href'].replace(".xml", ".txt") + "[]\n\n"


    
def book(el):
    b = ""
    elid = el.attrib.get('id')
    titleabbrev = el.find("./titleabbrev")
    if elid is not None:
        anchor = "\n[[" + strip_space(elid)
        if titleabbrev is not None:
            anchor += "," + titleabbrev.text
        anchor += "]]\n"
        b += anchor
    b += ":doctype: book\n"
    level = 1
    if 'part' in el:
        level = level + 1
    for child in el:
        if child.tag == 'title':
            b += title(child, 1)
        elif child.tag == 'bookinfo':
            b += bookinfo(child)
        elif child.tag == 'part':
            b += part(child, level)
        elif child.tag == 'article':
            b += article(child, level)
        elif child.tag == 'chapter':
            b += chapter(child, level+1)
        elif child.tag == 'appendix':
            b += appendix(child)
        elif child.tag == 'titleabbrev' or child.tag == 'glossary':
            pass
        elif child.tag == '{http://www.w3.org/2001/XInclude}include':
            b += include(child)
        elif child.tag == 'index':
            pass
        else:
            raise Exception(child.tag + " in book")
        if child.tail:
            b += normalize(child.tail)
    return b

def termoption(el):
    t = ""
    if el.text is not None:
        t += el.text
    for child in el:
        if child.tag == 'replaceable':
            t += replaceable(child)
        elif child.tag == 'optional':
            t += "[" + termoption(child) + "]"
        elif child.tag == 'literal':
            t += termoption(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail is not None:
            t += child.tail
    return t
    
def prompt(el, literal=False):
    role = el.attrib.get('role')
    p = ""
    if not literal:
        p += "++"

    if role is not None:
        p += role.replace("@", "@")
        if role.startswith("root"):
            p += "#"
        else:
            p += "%"
    else:
        p += el.text
    p += " "
    if not literal:
        p += "++"
    return p

def term(el):
    t = ""
    if el.text is not None and el.text.strip() != "":
        t += normalize(el.text.lstrip())
    for child in el:
        if child.tag in ['literal', 'computeroutput', 'envar', 'package']:
            t += "++" + child.text + "++"
        elif child.tag == 'filename':
            t += filename(child)
        elif child.tag == 'replaceable':
            t += "'" + child.text + "'"
        elif child.tag == 'emphasis':
            t += emphasis(child)
        elif child.tag == 'option':
            t += termoption(child)
        elif child.tag == 'prompt':
            t += prompt(child)
        elif child.tag == 'ulink':
            t += ulink(child)
        elif child.tag == 'glossterm':
            t += child.text
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail is not None and child.text.strip() != "":
            t += normalize(child.tail)
    return t

def varlistentry(el):
    e = ''
    terms = []
    for child in el:
        if child.tag == 'term':
            terms.append(term(child))
        elif child.tag == 'listitem':
            e += "**" + ", ".join(terms) + "**::\n"  
            terms = []
            e += listitem(child, 4, " ") + "\n"
        else:
            raise Exception(child.tag + " in variablelist")
    return e

def variablelist(el):
    v = ""
    for child in el:
        if child.tag == 'varlistentry':
            v += varlistentry(child)
        else:
            raise Exception(child.tag + " in variablelist")
    return v

def articleinfo(el):
    a = ""
    for child in el:
        if child.tag == 'title':
            a += title(child, 1)
        elif child.tag == 'abstract':
            a += "\n--\n" + abstract(child) + "\n--\n"
        elif child.tag == 'author':
            a += "\n:author: " + child.text + "\n"
        elif child.tag in ['authorinitials', 'date', 'revhistory']:
            pass
        else:
            raise Exception(child.tag + " in " + el.tag)
    return a

def article(el, depth=1):
    a = ""
    seenPara = False
    elid = el.attrib.get('id')
    titleabbrev = el.find("./titleabbrev")
    if elid is not None:
        anchor = "[[" + strip_space(elid)
        if titleabbrev is not None:
            anchor += "," + titleabbrev.text
        anchor += "]]\n"
        a += anchor
    if depth == 1:
        a += ":doctype: article\n"
    for child in el:
        if child.tag != 'para' and seenPara:
            a += "\n--\n"
            seenPara = False
        if child.tag == 'title':
            a += title(child, 1)
        elif child.tag == 'articleinfo':
            a += articleinfo(child)
        elif child.tag == 'para':
            if not seenPara:
                a += "\n--\n"
                seenPara = True
            a += para(child) + "\n"
        elif child.tag == 'simpara':
            if not seenPara:
                a += "\n--\n"
                seenPara = True
            a += simpara(child) + "\n"
        elif child.tag == 'itemizedlist':
            if not seenPara:
                a += "\n--\n"
                seenPara = True
            a += itemizedlist(child) + "\n"
        elif child.tag == 'note':
            if not seenPara:
                a += "\n--\n"
                seenPara = True
            a += note(child) + "\n"
        elif child.tag == 'section':
            a += section(child, 2)
        elif child.tag == 'chapter':
            a += chapter(child)
        elif child.tag == 'titleabbrev' or child.tag == 'glossary':
            pass
        elif child.tag == '{http://www.w3.org/2001/XInclude}include':
            a += include(child)
        elif child.tag == 'informaltable':
            a += informaltable(child)
        elif child.tag == 'abstract':
            a += abstract(child)
        elif child.tag == 'variablelist':
            a += variablelist(child)
        elif child.tag == 'indexterm':
            pass
        else:
            raise Exception(child.tag + " in article")
        if child.tail:
            a += normalize(child.tail)
    if seenPara:
        a += "\n--\n"
    return a
            
def partintro(el):
    p = ""
    for child in el:
        if child.tag == 'abstract':
            p += abstract(child)
    return p
def reference(el):
    a = ""
    elid = el.attrib.get('id')
    titleabbrev = el.find("./titleabbrev")
    if elid is not None:
        anchor = "[[" + strip_space(elid)
        if titleabbrev is not None:
            anchor += "," + titleabbrev.text
        anchor += "]]\n"
        a += anchor
    for child in el:
        if child.tag == 'title':
            a += title(child, 1)
        elif child.tag == '{http://www.w3.org/2001/XInclude}include':
            a += "\n:leveloffset: 2\n"
            a += include(child)
            a += "\n:leveloffset: 0\n"
        elif child.tag == 'partintro':
            a += partintro(child)
        elif child.tag == 'refentry':
            a += refentry(child)
        else:
            raise Exception(child.tag + " in reference")
        if child.tail and child.tail.strip() != "":
            a += normalize(child.tail)
    return a
    
def refmeta(el):

    title = el.find("refentrytitle").text
    vol = '3pm'
    if el.find("manvolnum") is not None:
        vol = el.find("manvolnum").text
    author = ""
    if el.find("refmiscinfo[@class='author']") is not None:
        author = el.find("refmiscinfo[@class='author']").text


    r = "= " + title.upper() + "(" + vol + ") =\n"
    r += ":doctype: manpage\n"
    if author is not None:
        r += ":man source: " + author + "\n"

    return r

def refnamediv(el):
    refname = ""
    refpurpose = ""

    for child in el:
        if child.tag == 'refname':
            refname = child.text
        elif child.tag == 'refpurpose':
            refpurpose = child.text

    return "\n== NAME ==\n" + refname + " - " + refpurpose + "\n"

def arg(el):
    c = ''

    choice = el.attrib.get("choice")
    if choice is None:
        choice = 'opt'

    if choice == 'opt':
        c += "["

    if el.text is not None:
        c += "++" + normalize(el.text.strip()) + "++ "

    for child in el:
        if child.tag == 'replaceable':
            c += replaceable(child)
        elif child.tag == 'group':
            c += group(child)
        else:
            raise Exception(child.tag +  " in " + el.tag)


    if el.tail is not None and el.tail.strip() != "":
        c += normalize(el.tail)


    rep = el.attrib.get("rep")
    if rep == 'repeat':
        c += "..."
    if choice == 'opt':
        c += "]"
    return c

def group(el):
    args = []
    for child in el:
        if child.tag == 'arg':
            args.append(arg(child))
        else:
            raise Exception(child.tag + " in " + el.tag)

    return " | ".join(args)

def cmdsynopsis(el):
    c = ""
    for child in el:
        if child.tag == 'command':
            c += command(child)
        elif child.tag == 'arg':
            c += arg(child)
        elif child.tag == 'sbr':
            c += " +\n +\n"
        elif child.tag == 'replaceable':
            c += replaceable(child)
        elif child.tag == 'group':
            c += group(child)
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail is not None:
            c += normalize(child.tail)
    return c
    
def refsynopsisdiv(el):
    refname = ""
    refpurpose = ""

    for child in el:
        if child.tag == 'cmdsynopsis':
            return "\n== SYNOPSIS ==\n" + cmdsynopsis(child) + "\n"

    return "\n== NAME ==\n" + refname + " - " + refpurpose + "\n"

def refentry(el):
    r = ""
    elid = el.attrib.get('id')
    titleabbrev = el.find("./titleabbrev")
    if elid is not None:
        anchor = "[[" + strip_space(elid)
        if titleabbrev is not None:
            anchor += "," + titleabbrev.text
        anchor += "]]\n"
        r += anchor
    for child in el:
        if child.tag == 'refmeta':
            r += refmeta(child)
        elif child.tag == 'refnamediv':
            r += refnamediv(child)
        elif child.tag == 'refsynopsisdiv':
            r += refsynopsisdiv(child)
        elif child.tag == 'refsection':
            r += section(child, 2)
        elif child.tag == 'refsect1':
            r += section(child, 2)
        elif child.tag == 'refentryinfo':
            if child.find("corpauthor") is not None:
                r += "\n:man source: " + child.find("corpauthor").text + "\n"
        else:
            raise Exception(child.tag + " in " + el.tag)
        if child.tail and child.tail.strip() != "":
            r += normalize(child.tail)
    return r

opts, rest = getopt.getopt(sys.argv[1:], "t:")

for (o,v) in opts:
    if o == '-t':
        toplevel_doc = v + "/"
for fn in rest:
    outfn = fn.replace(".xml", ".txt")
    out = open(outfn, "w")

    tree = ET.parse(fn)
    root = tree.getroot()

    d = ""
    if root.tag == 'book':
        d = book(root)
    elif root.tag == 'chapter':
        d = chapter(root)
    elif root.tag == 'article':
        d = article(root)
    elif root.tag == 'section':
        d = section(root, 3)
    elif root.tag == 'variablelist':
        d = variablelist(root)
    elif root.tag == 'reference':
        d = reference(root)
    elif root.tag == 'refentry':
        d = refentry(root)
    elif root.tag == 'para':
        d = para(root)
    elif root.tag == 'appendix':
        d = appendix(root)
    elif root.tag == 'bookinfo':
        d = bookinfo(root)
    elif root.tag == 'table':
        d = table(root)
    else:
        raise Exception(root.tag + " as root")

    out.write(unicode(d).encode('utf-8'))
    out.close()
