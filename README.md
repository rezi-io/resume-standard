<!-- PROJECT LOGO -->
<br />
<p align="center">

<h3 align="center">Resume Metadata Standard</h3>

  <p align="center">
    An ATS compliant standard for all resumes
    <br />
    <a href="https://resume-metadata-standard.com/">View Demo</a>
    ·
    <a href="https://github.com/rezi-io/resume-standard/issues">Report Bug</a>
    ·
    <a href="https://github.com/rezi-io/resume-standard/issues">Request Feature</a>
  </p>
</p> 

<!-- TABLE OF CONTENTS --> 
<details open="open">
  <summary><h2 style="display: inline-block">Table of Contents</h2></summary>
  <ol>
    <li><a href="#purpose">Purpose</a></li>
    <li><a href="#comparison-between-programmatic-parser-nlp-llm-parser-and-metadata-parser">Comparison</a></li>
    <li><a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#setting-up-the-parser">Setting Up the Parser</a></li>
        <li><a href="#using-the-parser">Using the Parser</a></li>
        <li><a href="#example-output">Example Output</a></li>
        <li><a href="#parser-logic">Parser Logic</a></li>
        <li><a href="#validating-and-using-the-data">Validating and Using the Data</a></li>
      </ul>
    </li>
    <li><a href="#schema">Schema</a>
      <ul>
        <li><a href="#schema-structure">Schema Structure</a></li>
        <li><a href="#essential-fields">Essential Fields</a></li>
        <li><a href="#contact">Contact</a></li>
        <li><a href="#summary">Summary</a></li>
        <li><a href="#experience">Experience</a></li>
        <li><a href="#education">Education</a></li>
        <li><a href="#certification">Certification</a></li>
        <li><a href="#coursework">Coursework</a></li>
        <li><a href="#involvement">Involvement</a></li>
        <li><a href="#project">Project</a></li>
        <li><a href="#skill">Skill</a></li>
        <li><a href="#publication">Publication</a></li>
        <li><a href="#award">Award</a></li>
        <li><a href="#reference">Reference</a></li>
      </ul>
    </li>
    <li><a href="#additional-documentation">Additional Documentation</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contributor">Contributor</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
  </ol>
</details>



<!-- PURPOSE -->
## Purpose

- PDF resume are not parsable by nature.
- Only standard format resume is compatible with ATS "Applicant Tracking System".
- Format creativity is prohibited in favor of ability to be parsable.  

This standard want change it by **allowing full creativity in resume format and structured data in metadata**.

Using an [XMP](https://en.wikipedia.org/wiki/Extensible_Metadata_Platform) "Extensible Metadata Platform" structure, common to all resume, make all resume parsable.

### When to Use:
- **As a first pass when a user imports a resume**: If structured standardized metadata exists, it can be used for **100% accuracy** and **fast output**.
- Checking can be done in less than **5ms**.
- It is up to the platform to decide which information they want to extract and utilize from the metadata.

### Who is Currently Using It:
- **[Rezi](https://www.rezi.ai), [Resumatic](https://www.resumatic.ai/), [Resumai](https://www.resumai.com), [ResumeBuild](https://www.resumebuild.ai)**, and others ([Add your company here](mailto:support@rezi.io))
- **Over 4 million resumes** already generated with this metadata on the Rezi platform.
- **Can be implemented in any resume builder, ATS, or HR platform** to be compliant with this standard (see section [How to Implement](/resume-builder/README.md)).


<!-- COMPARISON -->
## Comparison between Programmatic Parser, NLP/LLM Parser, and Metadata Parser

| **Criterion**                         | **Programmatic Parser**                                         | **NLP/LLM Parser**                        | **Metadata Parser**                                                                |
|---------------------------------------|-----------------------------------------------------------------|-------------------------------------------|------------------------------------------------------------------------------------|
| **Speed**                             | High                                                            | Slow                                      | Very fast                                                                          |
| **Cost**                              | Low                                                             | High                                      | Practically zero                                                                   |
| **Maintenance**                       | High (requires manual adjustments for different formats)        | Low (easier to maintain with pre-trained models) | None (standardized and requires no maintenance)                                    |
| **Output Quality on Simple Resumes**  | Moderate                                                        | High (accurate extraction of raw data)    | 100% accuracy (structured data is directly transferred)                            |
| **Output Quality on Complex Resumes** | Low                                                             | High                                      | 100% accuracy (even with unstructured resumes)                                     |
| **Accuracy**                          | Moderate                                                        | Can hallucinate or modify data            | 100% accuracy (data is faithfully extracted as metadata)                           |
| **Adaptability**                      | Very low (limited to fixed formats)                             | Fairly high (adapts to many formats)      | Nearly zero (focused on structured data, doesn’t adapt to various CV formats)      |
| **Ideal Use Case**                    | Bulk data analysis, requires manual post processing adjustments | Handles a variety of formats, but can be costly for large scale | Best as a first pass to check for structured data in resumes, reduce cost at scale |


<!-- GETTING STARTED -->
## Getting Started

### Setting Up the Parser

1. Clone the repository:
```bash
git clone https://github.com/rezi-io/resume-standard.git
cd resume-standard
```

2. Build the parser
   (tested with Docker 27.5.1):
- for local use:
```bash
g++ -std=c++17 -o ./parser/lib/rms-parser ./parser/src/parser.cpp
```
- for ARM architecture :
```bash
docker run --rm -v "$PWD":/src -w /src ubuntu:20.04 bash -c \                       
  "apt update && apt install -y g++ && g++ -o ./parser/lib/rms-parser ./parser/src/parser.cpp"
```

- for x86 architecture ( like Cloud functions, AWS Lambda, etc):
```bash
docker run --rm --platform linux/amd64 -v "$PWD":/src -w /src ubuntu:20.04 bash -c \
  "apt update && apt install -y g++ && g++ -o ./parser/lib/rms-parser ./parser/src/parser.cpp"
```


### Using the Parser

Run the parser against a PDF file to extract the Resume Metadata Standard (RMS) information:
It can be use with a Buffer or a file.

```bash
./parser/lib/rms-parser ./path/to/your/resume.pdf
```

Example:
```bash
./parser/lib/rms-parser ./pdf-examples/Software\ Engineer.pdf
```

The parser will first check for the `Producer` metadata. If it contains "rms_v2", the parser will extract and return all available metadata that conforms to the Resume Metadata Standard.

### Example Output

When you run the parser on a PDF with RMS metadata, you'll get JSON output similar to this:

```json
{
  "status": "success",
  "data": {
    "Description rdf:about": "",
    "Producer": "rms_v2.0.1",
    "pdf": "http://ns.adobe.com/pdf/1.3/",
    "producer": "rms_v2.0.1",
    "rms": "https://github.com/rezi-io/resume-standard",
    
    "rms_contact_city": "New York City",
    "rms_contact_country": "United States",
    "rms_contact_email": "charlesbloomberg@wisc.edu",
    "rms_contact_fullName": "Charles Bloomberg",
    "rms_contact_github": "n/a",
    "rms_contact_givenNames": "Charles",
    "rms_contact_lastName": "Bloomberg",
    "rms_contact_linkedin": "in/bloomberg",
    "rms_contact_phone": "(621) 799-5548",
    "rms_contact_state": "New York",
    "rms_contact_website": "n/a",
    
    "rms_education_0_date": "2021",
    "rms_education_0_dateFormat": "YYYY",
    "rms_education_0_dateTS": "1609459200000",
    "rms_education_0_description": "n/a",
    "rms_education_0_institution": "New York University",
    "rms_education_0_isGraduate": "true",
    "rms_education_0_location": "New York, NY",
    "rms_education_0_minor": "Computer Science",
    "rms_education_0_qualification": "Bachelor of Science in Biochemistry",
    "rms_education_0_score": "n/a",
    "rms_education_0_scoreType": "n/a",
    "rms_education_count": "1",
    
    "rms_experience_0_company": "Company B",
    "rms_experience_0_dateBegin": "June 2020",
    "rms_experience_0_dateBeginFormat": "MMMM YYYY",
    "rms_experience_0_dateBeginTS": "1590969600000",
    "rms_experience_0_dateEnd": "June 2021",
    "rms_experience_0_dateEndFormat": "MMMM YYYY",
    "rms_experience_0_dateEndTS": "1622505600000",
    "rms_experience_0_description": "• Created and maintained cloud-based service endpoints with Python, Flask, &amp; Django, increasing service uptime to 99% for an early-stage machine vision startup.\n• Designed &amp; developed intuitive UIs for internal users to train &amp; deploy new machine learning models, decreasing manual set-up time by 90%.\n• Developed embedded software for an ML-powered camera product, achieving 10X system throughput using concurrent programming.\n• Generated &amp; curated visual AI training data and tested deep learning systems to ensure 90% accuracy with &lt;10% training data compared with competitors.",
    "rms_experience_0_isCurrent": "false",
    "rms_experience_0_location": "New York, NY",
    "rms_experience_0_role": "Software Engineer Intern",
    "rms_experience_1_company": "Company C",
    "rms_experience_1_dateBegin": "June 2019",
    "rms_experience_1_dateBeginFormat": "MMMM YYYY",
    "rms_experience_1_dateBeginTS": "1559347200000",
    "rms_experience_1_dateEnd": "June 2020",
    "rms_experience_1_dateEndFormat": "MMMM YYYY",
    "rms_experience_1_dateEndTS": "1590969600000",
    "rms_experience_1_description": "• Bootstrapped project to automate reporting while part of a team of hazardous waste management specialists, eliminating 25% of reporting labor using custom UIs and 3rd-party API calls.\n• Wrote one-off scripts to automate revisions of thousands of legacy reports, cutting manual editing hours by 80%.",
    "rms_experience_1_isCurrent": "false",
    "rms_experience_1_location": "New York, NY",
    "rms_experience_1_role": "Script Programmer",
    "rms_experience_2_company": "Company A",
    "rms_experience_2_dateBegin": "June 2021",
    "rms_experience_2_dateBeginFormat": "MMMM YYYY",
    "rms_experience_2_dateBeginTS": "1622505600000",
    "rms_experience_2_dateEnd": "Present",
    "rms_experience_2_dateEndTS": "n/a",
    "rms_experience_2_description": "• Bootstrapped &amp; led a team of 4 developers to modernize fulfillment automation using a novel human-in-the-loop approach, unlocking an 80% increase in operational scale for a 40-person team.\n• Increased software development velocity by mentoring 4 developers and facilitating user interviews, leading to consistent resolution of 90% of bug reports within 24 hours.\n• Built and maintained distributed systems that served 1M daily requests, coordinating with operations, business, technical, and customer teams to serve cross-functional priorities.\n• Built CI/CD pipelines to lint, build, test, review, and deploy containerized applications to production &lt;30 minutes after development with Docker, GitHub Actions, and Heroku.\n• Led initiative in a team of 8 engineers to improve engineering on-call system, reducing the number of alerts per shift by 50% without dropping mission-critical information.\n• Streamlined critical operational processes to achieve a 6% boost in productivity for a team of 40 operators.",
    "rms_experience_2_isCurrent": "true",
    "rms_experience_2_location": "New York, NY",
    "rms_experience_2_role": "Software Engineer",
    "rms_experience_count": "3",
    
    "rms_project_0_description": "• Prototyped and deployed a music-based web app in 2 days using React, Flask, and AWS EC2.\n• Hosted the code on GitHub.",
    "rms_project_0_organization": "n/a",
    "rms_project_0_title": "Web-Based Musical Instrument",
    "rms_project_count": "1",
    
    "rms_schema_details": "https://github.com/rezi-io/resume-standard",
    
    "rms_skill_0_category": "Languages",
    "rms_skill_0_keywords": "Python, JavaScript, SQL",
    "rms_skill_1_category": "Frameworks",
    "rms_skill_1_keywords": "Flask, Django, Node,  Web extensions",
    "rms_skill_2_category": "Tools",
    "rms_skill_2_keywords": "Unix, Bash, GitHub, GitHub Actions, Docker, Cloud, Heroku, CI/CD, PostgreSQL",
    "rms_skill_count": "3",
    
    "rms_summary": "n/a",
    "xmpmeta:x:xmpmeta xmlns:x": "adobe:ns:meta/",
    "xmpmeta:x:xmptk": "Image::ExifTool 10.68"
  }
}

```

### Parser Logic

The basic logic of the parser includes:

1. Read a PDF file
2. Extract the XMP metadata
3. Check if the `Producer` field contains "rms_v2"
4. If found, extract all metadata fields that match the Resume Metadata Standard schema
5. Return the results as JSON

### Validating and Using the Data

After extracting the metadata, you can validate it against the schema and use it in your application. Common use cases include:

- Automatically populating application forms
- Pre-filling resume builder templates
- Creating standardized resume databases
- Improving ATS compatibility



## Schema

This standardized key structure makes the metadata easy to parse programmatically and provides a clear organization for resume data. It also ensures compatibility across different implementations of the Resume Metadata Standard.

### Schema Structure

All metadata keys in the Resume Metadata Standard follow a consistent naming structure to ensure clarity and organization of data. The structure follows this pattern:

```
rms_{section}_{index}_{field}
```

Where:
- **rms**: Prefix that stands for "Resume Metadata Standard", identifying all keys as part of this standard
- **{section}**: The section of the resume (e.g., experience, education, skill)
- **{index}**: A zero-based numerical index for items within a section (e.g., 0, 1, 2)
- **{field}**: The specific attribute of the item (e.g., title, description, date)

#### Examples:

- `rms_experience_0_company` - The company name for the first experience entry
- `rms_education_1_institution` - The institution name for the second education entry
- `rms_skill_2_keywords` - The keywords for the third skill category

#### Special Cases:

1. **Count fields**: These keys follow a slightly different pattern and do not include an index:
   ```
   rms_{section}_count
   ```
   For example: `rms_experience_count` indicates the total number of experience entries.

2. **Contact information**: Since contact details have only one instance, they don't require an index:
   ```
   rms_contact_{field}
   ```
   For example: `rms_contact_email` contains the person's email address.

3. **Summary field**: Similarly, the summary is a single entry without an index:
   ```
   rms_summary
   ```

### Essential fields
| Namespace         | Type | Example    |
|:------------------|:-----|:-----------|
| Producer          | Text | rms_v2.0.1 |
| rms_schema_detail | Text | https://github.com/rezi-io/resume-standard|

Version of the standard used to generate the metadata. This field is essential for the parser to understand the structure of the metadata.
You can review changelog [here](CHANGELOG.md)

### Contact
| Namespace               | Type                | Example                     |
|:------------------------|:--------------------|:----------------------------|
| rms_contact_fullName    | Text                | Charles Bloomberg           |
| rms_contact_givenNames  | Text                | Charles                     |
| rms_contact_lastName    | Text                | Bloomberg                   |
| rms_contact_email       | Text                | charlesbloomberg@email.com  |
| rms_contact_phone       | Text                | (621) 799-5548              |
| rms_contact_linkedin    | Text                | in/bloomberg                |
| rms_contact_github      | Text                | github.com/charlesbloomberg |
| rms_contact_behance     | Text                | github.com/charlesbloomberg |
| rms_contact_dribble     | Text                | github.com/charlesbloomberg |
| rms_contact_website     | Text                | www.charlesbloomberg.com    |
| rms_contact_country     | Text                | United States               |
| rms_contact_countryCode | Text (ISO 3166 A-2) | US                          |
| rms_contact_city        | Text                | Madison                     |
| rms_contact_state       | Text                | Wisconsin                   |

### Summary
| Namespace   | Type | Example |
|:------------|:-----|:--------|
| rms_summary | Text | Software Engineer with years of experience.... |

### Experience
| Namespace                                                                                                                                                  | Type    | Example                                                                                                                                                                               |
|:-----------------------------------------------------------------------------------------------------------------------------------------------------------|:--------|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| rms_experience_count                                                                                                                                       | Integer | 3<a href="#footnote-experience"><sup>1</sup></a>                                                                                                                                      |
| rms_experience_0_company<br/>rms_experience_1_company<br/>rms_experience_2_company<br/>...<br/>rms_experience_<a href="#footnote-experience">N</a>_company | Text    | Sony                                                                                                                                                                                  |
| <span style="white-space:nowrap">rms_experience_<a href="#footnote-experience">N</a>_description</span>                                                    | Text    | • Negotiated best pricing for hardware and software quotes and procurement.<br/>• Realigned and managed Sprint Wireless and XO Communication vendor accounts to fit business needs... |
| <span style="white-space:nowrap">rms_experience_<a href="#footnote-experience">N</a>_dateBegin</span>                                                      | Text    | June 2019                                                                                                                                                                             |
| <span style="white-space:nowrap">rms_experience_<a href="#footnote-experience">N</a>_dateBeginTS</span>                                                    | Integer | 1559347200000                                                                                                                                                                         |
| <span style="white-space:nowrap">rms_experience_<a href="#footnote-experience">N</a>_dateEnd</span>                                                        | Text    | June 2020                                                                                                                                                                             |
| <span style="white-space:nowrap">rms_experience_<a href="#footnote-experience">N</a>_dateEndTS</span>                                                      | Integer | 1590969600000                                                                                                                                                                         |
| <span style="white-space:nowrap">rms_experience_<a href="#footnote-experience">N</a>_isCurrent</span>                                                      | Bool    | false                                                                                                                                                                                 |
| <span style="white-space:nowrap">rms_experience_<a href="#footnote-experience">N</a>_role</span>                                                           | Text    | IT Support Specialist                                                                                                                                                                 |
| <span style="white-space:nowrap">rms_experience_<a href="#footnote-experience">N</a>_location</span>                                                       | Text    | Palo Alto, CA                                                                                                                                                                         |
<hr/>
<p><a id="footnote-experience"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

### Education
| Namespace                                                                                                                                                          | Type    | Example                                |
|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------|:--------|:---------------------------------------|
| rms_education_count                                                                                                                                                | Text | 3<a href="#footnote-education"><sup>1</sup></a> |
| rms_education_0_institution<br/>rms_education_1_institution<br/>rms_education_2_institution<br/>...<br/>rms_education_<a href="#footnote-education">N</a>_institution | Text    | University of Wisconsin - Madison      |
| <span style="white-space:nowrap">rms_education_<a href="#footnote-education">N</a>_qualification</span>                                                            | Text    | Bachelors of Science in Computer Science |
| <span style="white-space:nowrap">rms_education_<a href="#footnote-education">N</a>_location</span>                                                                 | Text    | Madison, Wisconsin                     |
| <span style="white-space:nowrap">rms_education_<a href="#footnote-education">N</a>_date</span>                                                                     | Text    | 2014                                  |
| <span style="white-space:nowrap">rms_education_<a href="#footnote-education">N</a>_dateTS</span>                                                                   | Text | 1388534400000                         |
| <span style="white-space:nowrap">rms_education_<a href="#footnote-education">N</a>_dateFormat</span>                                                               | Text    | YYYY                                  |
| <span style="white-space:nowrap">rms_education_<a href="#footnote-education">N</a>_isGraduate</span>                                                               | Text    | true                                  |
| <span style="white-space:nowrap">rms_education_<a href="#footnote-education">N</a>_minor</span>                                                                    | Text    | Mathematics                           |
| <span style="white-space:nowrap">rms_education_<a href="#footnote-education">N</a>_score</span>                                                                    | Text    | 3.81                                  |
| <span style="white-space:nowrap">rms_education_<a href="#footnote-education">N</a>_scoreType</span>                                                                | Text    | GPA                                   |
<hr/>
<p><a id="footnote-education"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

### Certification
| Namespace                                                                                                                                                                      | Type    | Example                                 |
|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:--------|:----------------------------------------|
| rms_certification_count                                                                                                                                                        | Text | 3<a href="#footnote-certification"><sup>1</sup></a> |
| rms_certification_0_name<br/>rms_certification_1_name<br/>rms_certification_2_name<br/>...<br/>rms_certification_<a href="#footnote-certification">N</a>_name                 | Text    | Project Management Professional (PMP)   |
| <span style="white-space:nowrap">rms_certification_<a href="#footnote-certification">N</a>_department</span>                                                                  | Text    | Project Management Institute            |
| <span style="white-space:nowrap">rms_certification_<a href="#footnote-certification">N</a>_date</span>                                                                        | Text    | 2014                                   |
| <span style="white-space:nowrap">rms_certification_<a href="#footnote-certification">N</a>_dateFormat</span>                                                                  | Text    | YYYY                                   |
| <span style="white-space:nowrap">rms_certification_<a href="#footnote-certification">N</a>_description</span>                                                                 | Text    | • Certified in a standardized and evolving set of project management principles. |
<hr/>
<p><a id="footnote-certification"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

### Coursework
| Namespace                                                                                                                                                      | Type    | Example                                |
|:---------------------------------------------------------------------------------------------------------------------------------------------------------------|:--------|:---------------------------------------|
| rms_coursework_count                                                                                                                                          | Text | 3<a href="#footnote-coursework"><sup>1</sup></a> |
| rms_coursework_0_name<br/>rms_coursework_1_name<br/>rms_coursework_2_name<br/>...<br/>rms_coursework_<a href="#footnote-coursework">N</a>_name               | Text    | Introduction to Computer Systems       |
| <span style="white-space:nowrap">rms_coursework_<a href="#footnote-coursework">N</a>_department</span>                                                        | Text    | University of Wisconsin, Madison       |
| <span style="white-space:nowrap">rms_coursework_<a href="#footnote-coursework">N</a>_date</span>                                                              | Text    | 2017                                  |
| <span style="white-space:nowrap">rms_coursework_<a href="#footnote-coursework">N</a>_dateTS</span>                                                            | Text | 1483228800000                         |
| <span style="white-space:nowrap">rms_coursework_<a href="#footnote-coursework">N</a>_dateFormat</span>                                                        | Text    | YYYY                                  |
| <span style="white-space:nowrap">rms_coursework_<a href="#footnote-coursework">N</a>_description</span>                                                       | Text    | • Coordinating on code with a small group of people. |
| <span style="white-space:nowrap">rms_coursework_<a href="#footnote-coursework">N</a>_skill</span>                                                             | Text    | Teamwork                              |
<hr/>
<p><a id="footnote-coursework"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

### Involvement
| Namespace                                                                                                                                                                | Type    | Example                                |
|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:--------|:---------------------------------------|
| rms_involvement_count                                                                                                                                                    | Text | 3<a href="#footnote-involvement"><sup>1</sup></a> |
| rms_involvement_0_organization<br/>rms_involvement_1_organization<br/>rms_involvement_2_organization<br/>...<br/>rms_involvement_<a href="#footnote-involvement">N</a>_organization | Text    | Economics Student Association          |
| <span style="white-space:nowrap">rms_involvement_<a href="#footnote-involvement">N</a>_location</span>                                                                    | Text    | University of Wisconsin, Madison       |
| <span style="white-space:nowrap">rms_involvement_<a href="#footnote-involvement">N</a>_dateBegin</span>                                                                   | Text    | June 2014                             |
| <span style="white-space:nowrap">rms_involvement_<a href="#footnote-involvement">N</a>_dateBeginTS</span>                                                                 | Text | 1401580800000                         |
| <span style="white-space:nowrap">rms_involvement_<a href="#footnote-involvement">N</a>_dateBeginFormat</span>                                                             | Text    | MMMM YYYY                             |
| <span style="white-space:nowrap">rms_involvement_<a href="#footnote-involvement">N</a>_dateEnd</span>                                                                     | Text    | September 2016                        |
| <span style="white-space:nowrap">rms_involvement_<a href="#footnote-involvement">N</a>_dateEndTS</span>                                                                   | Text | 1472688000000                         |
| <span style="white-space:nowrap">rms_involvement_<a href="#footnote-involvement">N</a>_dateEndFormat</span>                                                               | Text    | MMMM YYYY                             |
| <span style="white-space:nowrap">rms_involvement_<a href="#footnote-involvement">N</a>_role</span>                                                                        | Text    | Selected Member                       |
| <span style="white-space:nowrap">rms_involvement_<a href="#footnote-involvement">N</a>_description</span>                                                                 | Text    | • Participated in forums and discussions presented by key economic thinkers and companies associated with the university. |
<hr/>
<p><a id="footnote-involvement"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

### Project
| Namespace                                                                                                                                            | Type    | Example                                |
|:-----------------------------------------------------------------------------------------------------------------------------------------------------|:--------|:---------------------------------------|
| rms_project_count                                                                                                                                   | Text | 3<a href="#footnote-project"><sup>1</sup></a> |
| rms_project_0_title<br/>rms_project_1_title<br/>rms_project_2_title<br/>...<br/>rms_project_<a href="#footnote-project">N</a>_title                | Text    | Volunteer                              |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_organization</span>                                                  | Text    | Habitat for Humanity                   |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_role</span>                                                          | Text    | Administrator                         |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_dateBegin</span>                                                     | Text    | June 2014                             |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_dateBeginTS</span>                                                   | Text | 1401580800000                         |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_dateBeginFormat</span>                                               | Text    | MMMM YYYY                             |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_dateEnd</span>                                                       | Text    | September 2016                        |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_dateEndTS</span>                                                     | Text | 1472688000000                         |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_dateEndFormat</span>                                                 | Text    | MMMM YYYY                             |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_description</span>                                                   | Text    | • Volunteered to help renovate a house and managed a team of 6. |
| <span style="white-space:nowrap">rms_project_<a href="#footnote-project">N</a>_url</span>                                                           | Text    | https://github.com/username/project-repository |
<hr/>
<p><a id="footnote-project"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

### Skill
| Namespace                                                                                                                                  | Type    | Example                                |
|:-------------------------------------------------------------------------------------------------------------------------------------------|:--------|:---------------------------------------|
| rms_skill_count                                                                                                                           | Text | 3<a href="#footnote-skill"><sup>1</sup></a> |
| rms_skill_0_category<br/>rms_skill_1_category<br/>rms_skill_2_category<br/>...<br/>rms_skill_<a href="#footnote-skill">N</a>_category    | Text    | Language                              |
| <span style="white-space:nowrap">rms_skill_<a href="#footnote-skill">N</a>_keywords</span>                                                | Text    | French, English, Korean               |
<hr/>
<p><a id="footnote-skill"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

### Publication
| Namespace                                                                                                                                                                    | Type    | Example                                |
|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:--------|:---------------------------------------|
| rms_publication_count                                                                                                                                                        | Text | 3<a href="#footnote-publication"><sup>1</sup></a> |
| rms_publication_0_title<br/>rms_publication_1_title<br/>rms_publication_2_title<br/>...<br/>rms_publication_<a href="#footnote-publication">N</a>_title                     | Text    | Machine Learning for Natural Language Processing |
| <span style="white-space:nowrap">rms_publication_<a href="#footnote-publication">N</a>_organization</span>                                                                   | Text    | IEEE Transactions on Neural Networks    |
| <span style="white-space:nowrap">rms_publication_<a href="#footnote-publication">N</a>_role</span>                                                                           | Text    | Lead Author                           |
| <span style="white-space:nowrap">rms_publication_<a href="#footnote-publication">N</a>_date</span>                                                                           | Text    | June 2021                             |
| <span style="white-space:nowrap">rms_publication_<a href="#footnote-publication">N</a>_dateTS</span>                                                                         | Text | 1622505600000                         |
| <span style="white-space:nowrap">rms_publication_<a href="#footnote-publication">N</a>_dateFormat</span>                                                                     | Text    | MMMM YYYY                             |
| <span style="white-space:nowrap">rms_publication_<a href="#footnote-publication">N</a>_description</span>                                                                    | Text    | • Researched and published a paper detailing new approaches to natural language processing using advanced neural network architectures. |
| <span style="white-space:nowrap">rms_publication_<a href="#footnote-publication">N</a>_type</span>                                                                           | Text    | Academic Journal                      |
<hr/>
<p><a id="footnote-publication"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

### Award
| Namespace                                                                                                                                          | Type    | Example                                |
|:---------------------------------------------------------------------------------------------------------------------------------------------------|:--------|:---------------------------------------|
| rms_award_count                                                                                                                                    | Text | 3<a href="#footnote-award"><sup>1</sup></a> |
| rms_award_0_title<br/>rms_award_1_title<br/>rms_award_2_title<br/>...<br/>rms_award_<a href="#footnote-award">N</a>_title                        | Text    | Excellence in Research Award          |
| <span style="white-space:nowrap">rms_award_<a href="#footnote-award">N</a>_organization</span>                                                     | Text    | Computer Science Department           |
| <span style="white-space:nowrap">rms_award_<a href="#footnote-award">N</a>_date</span>                                                             | Text    | May 2020                              |
| <span style="white-space:nowrap">rms_award_<a href="#footnote-award">N</a>_dateTS</span>                                                           | Text | 1588291200000                         |
| <span style="white-space:nowrap">rms_award_<a href="#footnote-award">N</a>_dateFormat</span>                                                       | Text    | MMMM YYYY                             |
| <span style="white-space:nowrap">rms_award_<a href="#footnote-award">N</a>_description</span>                                                      | Text    | • Awarded for outstanding contributions to the field of Computer Science Research during graduate studies. |
<hr/>
<p><a id="footnote-award"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

### Reference
| Namespace                                                                                                                                                | Type    | Example                                |
|:---------------------------------------------------------------------------------------------------------------------------------------------------------|:--------|:---------------------------------------|
| rms_reference_count                                                                                                                                      | Text | 3<a href="#footnote-reference"><sup>1</sup></a> |
| rms_reference_0_name<br/>rms_reference_1_name<br/>rms_reference_2_name<br/>...<br/>rms_reference_<a href="#footnote-reference">N</a>_name              | Text    | Dr. Jane Smith                        |
| <span style="white-space:nowrap">rms_reference_<a href="#footnote-reference">N</a>_phone</span>                                                         | Text    | (555) 123-4567                        |
| <span style="white-space:nowrap">rms_reference_<a href="#footnote-reference">N</a>_email</span>                                                         | Text    | jane.smith@university.edu             |
| <span style="white-space:nowrap">rms_reference_<a href="#footnote-reference">N</a>_type</span>                                                          | Text    | Professional                          |
| <span style="white-space:nowrap">rms_reference_<a href="#footnote-reference">N</a>_organization</span>                                                  | Text    | University of California, Berkeley    |
| <span style="white-space:nowrap">rms_reference_<a href="#footnote-reference">N</a>_role</span>                                                          | Text    | Department Chair                      |
<hr/>
<p><a id="footnote-reference"><sup>1</sup></a> : Integer from 0 to 15 giving the total number of items in this specific section.</p>

## Additional Documentation

### Data Formats

#### Date and Timestamp Formats
- **Date Display Format**: Different fields use different date display formats based on their context:
    - `YYYY` (e.g., "2021") - Used for education graduation dates and similar annual milestones
    - `MMMM YYYY` (e.g., "June 2021") - Used for most work experiences, projects, etc.
    - Other formats should be specified in the corresponding `dateFormat` field

- **Timestamp Fields**: All `dateTS` fields store Unix timestamps in milliseconds since epoch (January 1, 1970).
    - This provides a standardized way to store and compare dates programmatically
    - Example: `1622505600000` represents June 1, 2021, 00:00:00 UTC

#### URL and Social Media Fields
- **URL Fields**:
    - Standard URL fields (website, GitHub, etc.) should include full URLs when possible
    - For social media profiles, the username or handle format is acceptable:
        - LinkedIn: `in/username` instead of full `https://linkedin.com/in/username`
        - GitHub: `username` or `github.com/username`
        - Dribbble: `username` or `dribbble.com/username`
        - Behance: `username` or `behance.net/username`

#### Handling Missing Data
- **Empty Fields**: Use `n/a` for fields that are intentionally left blank rather than omitting them
    - This indicates that the field was considered but has no value
    - Example: `"rms_contact_github": "n/a"`
- **Unknown Dates**: For current positions where end date is unknown, use `Present` in the date field and omit the dateTS field

### Metadata Structure
- **XMP Structure**: The RMS uses Adobe's Extensible Metadata Platform (XMP) embedded in the PDF
    - XMP allows for structured metadata to be stored in the PDF file without altering its appearance
    - The metadata is organized into namespaces, with the RMS namespace identified by the "rms_" prefix
    - Multiple tools can read XMP data, not just ExifTool



<!-- LICENSE -->
## License 

Distributed under the MIT License.


<!-- Contributor -->
## Contributor
Rezi Development Team - support@rezi.io



<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements

* Exif Tool [https://exiftool.org/TagNames/XMP.html](https://exiftool.org/TagNames/XMP.html)
* XML by Adobe Systems Inc. [https://en.wikipedia.org/wiki/Extensible_Metadata_Platform](https://en.wikipedia.org/wiki/Extensible_Metadata_Platform)





<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->