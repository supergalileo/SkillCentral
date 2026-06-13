#include "ui/MarkdownRenderer.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

QString MarkdownRenderer::toHtml(const QString &markdown)
{
    if (markdown.isEmpty()) {
        return QString();
    }

    QString html = markdown;

    // 转换顺序很重要，需要先处理代码块（避免内部 Markdown 语法被转换）
    html = convertCodeBlocks(html);
    html = convertBlockquotes(html);
    html = convertHeaders(html);
    html = convertHorizontalRules(html);
    html = convertUnorderedLists(html);
    html = convertOrderedLists(html);
    html = convertBold(html);
    html = convertItalic(html);
    html = convertInlineCode(html);
    html = convertLinks(html);
    html = convertParagraphs(html);

    return html;
}

QString MarkdownRenderer::convertHeaders(const QString &md)
{
    QString result = md;

    // 一级标题 (# )
    QRegularExpression h1Regex("^# (.+)$", QRegularExpression::MultilineOption);
    result.replace(h1Regex, "<h1>\\1</h1>");

    // 二级标题 (## )
    QRegularExpression h2Regex("^## (.+)$", QRegularExpression::MultilineOption);
    result.replace(h2Regex, "<h2>\\1</h2>");

    // 三级标题 (### )
    QRegularExpression h3Regex("^### (.+)$", QRegularExpression::MultilineOption);
    result.replace(h3Regex, "<h3>\\1</h3>");

    // 四级标题 (#### )
    QRegularExpression h4Regex("^#### (.+)$", QRegularExpression::MultilineOption);
    result.replace(h4Regex, "<h4>\\1</h4>");

    // 五级标题 (##### )
    QRegularExpression h5Regex("^##### (.+)$", QRegularExpression::MultilineOption);
    result.replace(h5Regex, "<h5>\\1</h5>");

    // 六级标题 (###### )
    QRegularExpression h6Regex("^###### (.+)$", QRegularExpression::MultilineOption);
    result.replace(h6Regex, "<h6>\\1</h6>");

    return result;
}

QString MarkdownRenderer::convertBold(const QString &md)
{
    QString result = md;

    // 粗体 **text** 或 __text__
    QRegularExpression boldRegex(R"(\*\*(.+?)\*\*|__(.+?)__)");
    result.replace(boldRegex, "<b>\\1\\2</b>");

    return result;
}

QString MarkdownRenderer::convertItalic(const QString &md)
{
    QString result = md;

    // 斜体 *text* 或 _text_ （不处理已经被粗体处理过的部分）
    // 使用负向断言避免匹配粗体内部的内容
    QRegularExpression italicRegex(R"((?<!\*)\*(?!\*)(.+?)\*(?!\*)|(?<!_)_(?!_)(.+?)_(?!_))");
    result.replace(italicRegex, "<i>\\1\\2</i>");

    return result;
}

QString MarkdownRenderer::convertInlineCode(const QString &md)
{
    QString result = md;

    // 行内代码 `code`
    QRegularExpression codeRegex("`([^`]+)`");
    result.replace(codeRegex, "<code style='background-color:#f5f5f5;padding:2px 4px;border-radius:3px;font-family:monospace;'>\\1</code>");

    return result;
}

QString MarkdownRenderer::convertCodeBlocks(const QString &md)
{
    QString result = md;

    // 代码块 ```lang\ncode\n```
    // 使用 DotMatchesEverythingOption 让 . 匹配换行符
    QRegularExpression codeBlockRegex("```(?:(\\w+)\\n)?(.*?)```",
                                       QRegularExpression::DotMatchesEverythingOption);
    result.replace(codeBlockRegex,
                   "<pre style='background-color:#f5f5f5;padding:10px;"
                   "border-radius:4px;overflow-x:auto;'>"
                   "<code>\\2</code></pre>");

    return result;
}

QString MarkdownRenderer::convertUnorderedLists(const QString &md)
{
    QString result = md;
    QStringList lines = result.split('\n');
    QStringList output;
    bool inList = false;

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        QRegularExpression ulRegex("^\\s*[-*+]\\s+(.+)$");
        QRegularExpressionMatch match = ulRegex.match(line);

        if (match.hasMatch()) {
            if (!inList) {
                output.append("<ul>");
                inList = true;
            }
            output.append(QString("<li>%1</li>").arg(match.captured(1)));
        } else {
            if (inList) {
                output.append("</ul>");
                inList = false;
            }
            output.append(line);
        }
    }

    if (inList) {
        output.append("</ul>");
    }

    return output.join('\n');
}

QString MarkdownRenderer::convertOrderedLists(const QString &md)
{
    QString result = md;
    QStringList lines = result.split('\n');
    QStringList output;
    bool inList = false;

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        QRegularExpression olRegex("^\\s*\\d+\\.\\s+(.+)$");
        QRegularExpressionMatch match = olRegex.match(line);

        if (match.hasMatch()) {
            if (!inList) {
                output.append("<ol>");
                inList = true;
            }
            output.append(QString("<li>%1</li>").arg(match.captured(1)));
        } else {
            if (inList) {
                output.append("</ol>");
                inList = false;
            }
            output.append(line);
        }
    }

    if (inList) {
        output.append("</ol>");
    }

    return output.join('\n');
}

QString MarkdownRenderer::convertLinks(const QString &md)
{
    QString result = md;

    // 链接 [text](url)
    QRegularExpression linkRegex(R"(\[([^\]]+)\]\(([^\)]+)\))");
    result.replace(linkRegex, "<a href='\\2' style='color:#4a90d9;text-decoration:none;'>\\1</a>");

    return result;
}

QString MarkdownRenderer::convertHorizontalRules(const QString &md)
{
    QString result = md;

    // 水平线 --- 或 *** 或 ___
    QRegularExpression hrRegex("^[-*_]{3,}\\s*$", QRegularExpression::MultilineOption);
    result.replace(hrRegex, "<hr style='border:none;border-top:1px solid #e0e0e0;margin:10px 0;'>");

    return result;
}

QString MarkdownRenderer::convertBlockquotes(const QString &md)
{
    QString result = md;
    QStringList lines = result.split('\n');
    QStringList output;
    bool inQuote = false;

    for (int i = 0; i < lines.size(); ++i) {
        QString line = lines[i];
        QRegularExpression quoteRegex("^>\\s?(.*)$");
        QRegularExpressionMatch match = quoteRegex.match(line);

        if (match.hasMatch()) {
            if (!inQuote) {
                output.append("<blockquote style='border-left:4px solid #ccc;padding-left:10px;margin:5px 0;color:#666;'>");
                inQuote = true;
            }
            output.append(match.captured(1));
        } else {
            if (inQuote) {
                output.append("</blockquote>");
                inQuote = false;
            }
            output.append(line);
        }
    }

    if (inQuote) {
        output.append("</blockquote>");
    }

    return output.join('\n');
}

QString MarkdownRenderer::convertParagraphs(const QString &md)
{
    QString result = md;

    // 将连续两个换行转换为段落分隔
    // 但避免在已经是对 HTML 标签的行后面添加 <p> 标签
    QStringList lines = result.split('\n');
    QStringList output;
    bool firstLine = true;

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) {
            output.append("<br>");
        } else {
            // 检查是否是 HTML 标签行
            if (!trimmed.startsWith('<') || trimmed.startsWith("</")) {
                if (!firstLine) {
                    output.append("");
                }
            }
            output.append(line);
            firstLine = false;
        }
    }

    return output.join('\n');
}
