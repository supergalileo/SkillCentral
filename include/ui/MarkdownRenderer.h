#ifndef MARKDOWNRENDERER_H
#define MARKDOWNRENDERER_H

#include <QString>

/**
 * @brief 静态 Markdown 转 HTML 渲染器
 * 
 * 使用 QRegularExpression 将 Markdown 文本转换为 HTML 字符串，
 * 供 QTextBrowser::setHtml() 使用。
 * 
 * 支持：
 * - 标题 (# ## ###)
 * - 粗体 (**text**)
 * - 斜体 (*text*)
 * - 行内代码 (`code`)
 * - 代码块 (```code```)
 * - 无序列表 (- item)
 * - 有序列表 (1. item)
 * - 链接 ([text](url))
 * - 水平线 (---)
 * - 引用 (> text)
 */
class MarkdownRenderer
{
public:
    /**
     * @brief 将 Markdown 文本转换为 HTML
     * @param markdown 输入的 Markdown 文本
     * @return 转换后的 HTML 字符串
     */
    static QString toHtml(const QString &markdown);

private:
    // 阻止实例化
    MarkdownRenderer() = delete;

    /**
     * @brief 转换标题
     * @param md 包含标题的 Markdown 文本
     * @return 转换后的 HTML（标题部分）
     */
    static QString convertHeaders(const QString &md);

    /**
     * @brief 转换粗体
     */
    static QString convertBold(const QString &md);

    /**
     * @brief 转换斜体
     */
    static QString convertItalic(const QString &md);

    /**
     * @brief 转换行内代码
     */
    static QString convertInlineCode(const QString &md);

    /**
     * @brief 转换代码块
     */
    static QString convertCodeBlocks(const QString &md);

    /**
     * @brief 转换无序列表
     */
    static QString convertUnorderedLists(const QString &md);

    /**
     * @brief 转换有序列表
     */
    static QString convertOrderedLists(const QString &md);

    /**
     * @brief 转换链接
     */
    static QString convertLinks(const QString &md);

    /**
     * @brief 转换水平线
     */
    static QString convertHorizontalRules(const QString &md);

    /**
     * @brief 转换引用
     */
    static QString convertBlockquotes(const QString &md);

    /**
     * @brief 转换段落（处理换行）
     */
    static QString convertParagraphs(const QString &md);
};

#endif // MARKDOWNRENDERER_H
