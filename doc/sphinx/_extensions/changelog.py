from datetime import datetime

from docutils import nodes
from docutils.parsers.rst import directives
from sphinx.util.docutils import SphinxDirective


class ReleaseDirective(SphinxDirective):
    has_content = True
    required_arguments = 1
    optional_arguments = 0
    option_spec = {
        "date": directives.unchanged_required
    }

    def run(self):
        """Creates a section for release notes with version and date."""
        version = self.arguments[0]

        # Fetch today's date as default if no date is provided
        today_date_str = datetime.now().strftime("%Y-%m-%d")
        date_str = self.options.get("date", today_date_str)

        try:
            parsed_date = datetime.strptime(date_str, "%Y-%m-%d")
            release_date = parsed_date.strftime("%e %B %Y")
        except ValueError:
            raise ValueError(f"Invalid date format: {date_str}")

        # Create the version title node
        version_node = nodes.strong(text=version)
        section_title = nodes.title("", "", version_node)

        # Create the section node with a specific ID
        section_id = f"release-{version.replace(' ', '-')}"
        section = nodes.section(
            "", section_title,
            ids=[section_id],
            classes=["changelog-release"]
        )

        # Append formatted date
        section.append(
            nodes.emphasis(text=release_date, classes=["release-date"])
        )

        # Parse content into a list of changes
        content_node = nodes.Element()
        self.state.nested_parse(self.content, self.content_offset, content_node)

        # Create a bullet list of changes
        changes_list = nodes.bullet_list("", classes=["changelog-change-list"])
        for child in content_node:
            item = nodes.list_item("")
            item.append(child)
            changes_list.append(item)

        section.append(changes_list)

        return [section]


class ChangeDirective(SphinxDirective):
    has_content = True
    required_arguments = 1
    optional_arguments = 0

    def run(self):
        """Generates a categorized list item for a changelog entry."""
        category = self.arguments[0]

        # Create a paragraph for the category with specific styling
        class_name = f"changelog-category-{category.lower().replace(' ', '-')}"
        category_node = nodes.inline(
            "", category,
            classes=["changelog-category", class_name]
        )
        paragraph_node = nodes.paragraph("", "", category_node)

        # Parse the detailed content under the category
        content_node = nodes.container()
        self.state.nested_parse(self.content, 0, content_node)
        paragraph_node += content_node

        return [paragraph_node]


def setup(app):
    """Register extension with Sphinx."""
    app.add_directive("release", ReleaseDirective)
    app.add_directive("change", ChangeDirective)
